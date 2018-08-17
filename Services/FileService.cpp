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

bool FileService::extract(char* pattern, void* target) {
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
			case 'I':
				in >> *(int*)((char*)target + offset);
				// Not sure this is adequately robust, but it does work. Also puts all the responsibility on the caller to validate the results, but it already has all the responsibility for memory allocation, so this may not be a bad thing
				if (in.fail() && !strictlyNecessary) {
					*(int*)((char*)target + offset) = -1;
					in.clear();
				}
				offset += sizeof(int);
				break;
			case 'F':
				in >> *(float*)((char*)target + offset);
				if (in.fail() && !strictlyNecessary) {
					*(float*)((char*)target + offset) = NAN;
					in.clear();
				}
				offset += sizeof(float);
				break;
			case 'S':
			{
				tok++;
				int stringStart = in.tellg();
				while (in.peek() != pattern[tok]) {
					in.ignore(1);
					if (in.peek() == EOF) {
						in.seekg(pos, in.beg);
						return false;
					}
				}
				int stringEnd = in.tellg();
				int length = stringEnd - stringStart;
				in.seekg(stringStart, in.beg);
				char* string = new char[(int)length + 1];
				in.get(string, (int)length + 1);
				*(char**)((char*)target + offset) = string;
				offset += sizeof(char*);
				in.ignore(1);
			}
				break;
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
	return true;
}