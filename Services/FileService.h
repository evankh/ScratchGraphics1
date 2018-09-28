#ifndef __EKH_SCRATCH_GRAPHICS_1_FILE_SERVICE__
#define __EKH_SCRATCH_GRAPHICS_1_FILE_SERVICE__

#include <fstream>
#include <string>

class FileService {
private:
	std::ifstream in;
public:
	FileService(const char* filename);
	bool good() { return in.good(); };
	void close();
	std::string getAll();
	bool extract(char* pattern, void* target);
	void restart();
};

#endif//__EKH_SCRATCH_GRAPHICS_1_FILE_SERVICE__