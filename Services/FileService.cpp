#include "FileService.h"

FileService::FileService(const char* filename) {
	in.open(filename, std::ios::binary);
	mPath = filename;
	if (!in.good()) {
		in.close();
		throw FileNotFoundException(filename);
	}
}

FileService::FileService(std::string filename) :FileService(filename.data()) {}

FileService::~FileService() {
	in.close();
}

std::string FileService::getAll() {
	std::streamoff pos = in.tellg();
	in.seekg(0, in.beg);
	std::string buffer;
	std::getline(in, buffer, (char)EOF);
	in.clear();
	in.seekg(pos, in.beg);
	return buffer;
}

void FileService::restart() {
	in.clear();
	in.seekg(0, in.beg);
}

enum DelimType {
	DELIM_CHAR = 0,
	DELIM_WHITESPACE,
	DELIM_EOF,
	DELIM_LINE
};

bool found_delim(char c, DelimType type, char delim) {
	switch (type) {
	case DELIM_WHITESPACE:
		return c == ' ' || c == '\r' || c == '\n' || c == '\t' || c == EOF;
	case DELIM_EOF:
		return c == EOF;
	case DELIM_CHAR:
		return c == delim;
	case DELIM_LINE:
		return c == '\r' || c == '\n' || c == EOF;
	}
	return false;
}

int ceiling(int i, int step) {
	// Returns i rounded up to the next multiple of step. Step must be a power of 2.
	if (i & (step - 1))
		return (i & ~(step - 1)) + step;
	return i;
}

template <typename T>
T swapbytes(T i) {
	union { int8_t b[sizeof(T)]; T d; } in, out;
	in.d = i;
	for (int j = 0; j < sizeof(T); j++) out.b[j] = in.b[sizeof(T) - 1 - j];
	return out.d;
}

bool FileService::extract(const char* pattern, void* target) {
	std::streamoff pos = in.tellg();
	unsigned int offset = 0;
	for (unsigned int tok = 0; pattern[tok] != '\0'; tok++) {
		if (pattern[tok] == '`') {
			tok++;
			bool strictlyNecessary = true, raw = false;
			if (pattern[tok] == '?') {
				strictlyNecessary = false;
				tok++;
			}
			if (pattern[tok] == 'r') {
				raw = true;
				tok++;
			}
			switch (pattern[tok]) {
			case 'I': {
				offset = ceiling(offset, alignof(int));
				int temp;
				if (raw)
					in.read((char*)&temp, sizeof(int));
				else
					in >> temp;
				if (raw && mBackwards) temp = swapbytes(temp);
				// Not sure this is adequately robust, but it does work. Also puts all the responsibility on the caller to validate the results, but it already has all the responsibility for memory allocation, so this may not be a bad thing
				if (in.fail()) {
					in.clear();
					if (strictlyNecessary) {
						in.seekg(pos, in.beg);
						return false;
					}
					if (target) *(int*)((char*)target + offset) = -1;
				}
				else
					if (target) *(int*)((char*)target + offset) = temp;
				offset += sizeof(int);
				break;
			}
			case 'F': {
				offset = ceiling(offset, alignof(float));
				float temp;
				if (raw)
					in.read((char*)&temp, sizeof(float));
				else
					in >> temp;
				if (raw && mBackwards) temp = swapbytes(temp);
				if (in.fail()) {
					in.clear();
					if (strictlyNecessary) {
						in.seekg(pos, in.beg);
						return false;
					}
					if (target) *(float*)((char*)target + offset) = NAN;
				}
				else
					if (target) *(float*)((char*)target + offset) = temp;
				offset += sizeof(float);
				break;
			}
			case 's': {
				offset = ceiling(offset, alignof(short));
				short temp;
				if (raw)
					in.read((char*)&temp, sizeof(short));
				else
					in >> temp;
				if (raw && mBackwards) temp = swapbytes(temp);
				if (in.fail()) {
					in.clear();
					if (strictlyNecessary) {
						in.seekg(pos, in.beg);
						return false;
					}
					if (target) *(short*)((char*)target + offset) = -1;
				}
				else
					if (target) *(short*)((char*)target + offset) = temp;
				offset += sizeof(short);
				break;
			}
			case 'l': {
				offset = ceiling(offset, alignof(long long));
				long long temp;
				if (raw)
					in.read((char*)&temp, sizeof(long long));
				else
					in >> temp;
				if (raw && mBackwards) temp = swapbytes(temp);
				if (in.fail()) {
					in.clear();
					if (strictlyNecessary) {
						in.seekg(pos, in.beg);
						return false;
					}
					if (target) *(long long*)((char*)target + offset) = -1;
				}
				else
					if (target) *(long long*)((char*)target + offset) = temp;
				offset += sizeof(long long);
				break;
			}
			case 'C': {
				offset = ceiling(offset, alignof(char));
				int temp = in.get();
				if (target) *(char*)((char*)target + offset) = char(temp & 0xff);
				offset += sizeof(char);
				break;
			}
			case 'S': {
				offset = ceiling(offset, alignof(char*));
				std::streampos stringStart = in.tellg();
				char delim = pattern[tok + 1];
				DelimType type = DELIM_CHAR;
				if (delim == '`') {
					if (pattern[tok + 2] == 'W')	// Assumes a well-formatted pattern string, like everything else here
						type = DELIM_WHITESPACE;
					else if (pattern[tok + 2] == 'E')
						type = DELIM_EOF;
					else if (pattern[tok + 2] == 'L')
						type = DELIM_LINE;
					else
						delim = pattern[tok + 3];
				}	// Handle the special cases
				/*while (!found_delim(in.peek(), type, delim)) {
					in.ignore(1);
					if (in.peek() == EOF) {
						in.clear();
						if (type == DELIM_CHAR) {
							if (!strictlyNecessary) {
								if (target) *(char**)((char*)target + offset) = NULL;
								in.clear();
								in.ignore(1);
								offset += sizeof(char*);
								break;
							}
							else {
								in.seekg(pos, in.beg);
								return false;
							}
						}
					}
				}*/
				while (true) {
					if (in.peek() == EOF) {
						in.clear();
						if (type == DELIM_CHAR) {
							in.seekg(pos, in.beg);
							return false;
						}
						else
							break;
					}
					else {
						if (found_delim(in.peek(), type, delim))
							break;
						else
							in.ignore(1);
					}
				}
				std::streampos stringEnd = in.tellg();
				std::streamoff length = stringEnd - stringStart;
				if (length) {
					in.seekg(stringStart, in.beg);
					char* string = new char[(int)length + 1];
					in.get(string, (int)length + 1);
					if (target) *(char**)((char*)target + offset) = string;
					offset += sizeof(char*);
				}
				else {
					if (strictlyNecessary) {
						in.seekg(pos, in.beg);
						return false;
					}
					else {
						if (target) *(char**)((char*)target + offset) = NULL;
						offset += sizeof(char*);
					}
				}
				break;
			}
			case 'W':
				if (in.peek() == ' ' || in.peek() == '\t' || in.peek() == '\r' || in.peek() == '\n') {
					while (in.peek() == ' ' || in.peek() == '\t' || in.peek() == '\r' || in.peek() == '\n')	// Needs to check & handle EOF too
						in.get();
				}
				else if (strictlyNecessary) {
					in.seekg(pos, in.beg);
					return false;
				}
				break;
			case 'L':
				if (in.peek() == EOF)
					return true;
				if (in.peek() == '\r' || in.peek() == '\n') {
					while (in.peek() == '\r' || in.peek() == '\n')
						in.get();
				}
				else if (strictlyNecessary) {
					in.seekg(pos, in.beg);
					return false;
				}
				break;
			case 'E':
				if (in.peek() == EOF)
					return true;	// Should check to make sure the pattern stops here, but w/e, that's on the caller
				else if (strictlyNecessary) {
					in.seekg(pos, in.beg);
					return false;
				}
				break;
			case '.':
				in.get();
				break;
			default:
				break;
			}
		}
		else {
			char buffer = in.get();
			if (buffer != pattern[tok]) {
				in.clear();	// In case we got EOF (will this have any other consequences?)
				in.seekg(pos, in.beg);
				return false;
			}
		}
	}
	return true;
}

bool FileService::putBack(const char* pattern) {	// Begging for off-by-one errors, keep a close eye on it
	int length = 0;
	std::streampos pos = in.tellg();
	for (; pattern[length] != '\0'; length++);
	while (length > 0) {
		in.seekg(-1, in.cur);
		if (in.peek() != pattern[--length]) {
			in.seekg(pos);
			return false;
		}
	}
	return true;
}

bool FileService::putBack(const char pattern) {
	std::streampos pos = in.tellg();
	in.seekg(-1, in.cur);
	if (in.peek() == pattern)
		return true;
	in.seekg(pos);
	return false;
}

void FileService::jumpToPosition(int position) {
	in.seekg(position, in.beg);
}

void FileService::jumpToOffset(int offset) {
	in.seekg(offset, in.cur);
}

void FileService::reverseEndian() {
	mBackwards = !mBackwards;
}