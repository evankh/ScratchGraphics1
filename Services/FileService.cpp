#include "FileService.h"

FileService::FileService(const char* filename) {
	in.open(filename, std::ios::binary);
}

void FileService::close() {
	in.close();
	delete this;
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

bool FileService::extract(const char* pattern, void* target) {
	in.clear();	// DEBUG
	std::streamoff pos = in.tellg();
	unsigned int offset = 0;
	for (unsigned int tok = 0; pattern[tok] != '\0'; tok++) {
		if (pattern[tok] == '\\') {
			tok++;
			bool strictlyNecessary = true;
			if (pattern[tok] == '?') {
				strictlyNecessary = false;
				tok++;
			}
			switch (pattern[tok]) {
			case 'I': {
				int temp;
				in >> temp;
				// Not sure this is adequately robust, but it does work. Also puts all the responsibility on the caller to validate the results, but it already has all the responsibility for memory allocation, so this may not be a bad thing
				if (in.fail() && !strictlyNecessary) {
					if (target) *(int*)((char*)target + offset) = -1;
					in.clear();
				}
				if (target) *(int*)((char*)target + offset) = temp;
				offset += sizeof(int);
				break;
			}
			case 'F': {
				float temp;
				in >> temp;
				if (in.fail() && !strictlyNecessary) {
					if (target) *(float*)((char*)target + offset) = NAN;
					in.clear();
				}
				if (target) *(float*)((char*)target + offset) = temp;
				offset += sizeof(float);
				break;
			}
			case 'S': {
				tok++;
				std::streampos stringStart = in.tellg();
				while (in.peek() != pattern[tok]) {
					in.ignore(1);
					if (in.peek() == EOF) {
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
				std::streampos stringEnd = in.tellg();
				std::streamoff length = stringEnd - stringStart;
				if (length) {
					in.seekg(stringStart, in.beg);
					char* string = new char[(int)length + 1];
					in.get(string, (int)length + 1);
					if (target) *(char**)((char*)target + offset) = string;
					offset += sizeof(char*);
					in.ignore(1);
				}
				else {
					if (strictlyNecessary) {
						in.seekg(pos, in.beg);
						return false;
					}
					else {
						if (target) *(char**)((char*)target + offset) = NULL;
						offset += sizeof(char*);
						in.ignore(1);
					}
				}
				break;
			}
			case 'W':
				while (in.peek() == ' ' || in.peek() == '\t' || in.peek() == '\r' || in.peek() == '\n')
					in.get();
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
			if (buffer == '\r')
				buffer = in.get();	// Screw line endings
			if (buffer != pattern[tok]) {
				in.seekg(pos, in.beg);
				return false;
			}
		}
	}
	in.good();
	in.clear();
	return true;
}

bool FileService::putBack(const char* pattern) {	// Begging for off-by-one errors, keep aclose eye on it
	int length = 0;
	std::streampos pos = in.tellg();
	for (; pattern[length] != '\0'; length++);
	while (length > 0) {
		if (in.peek() == pattern[--length])
			in.seekg(-1, in.cur);
		else {
			in.seekg(pos);
			return false;
		}
	}
	return true;
}