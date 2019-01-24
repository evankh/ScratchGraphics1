#ifndef __EKH_SCRATCH_GRAPHICS_1_FILE_SERVICE__
#define __EKH_SCRATCH_GRAPHICS_1_FILE_SERVICE__

#include <fstream>
#include <string>

class FileService {
private:
	std::ifstream in;
	std::string mPath;
public:
	FileService(const char* filename);
	FileService(std::string filename);
	~FileService();
	bool good() { return in.good(); };
	const std::string getPath() const { return mPath; };
	std::string getAll();
	bool extract(const char* pattern, void* target);
	void restart();
	bool putBack(const char* pattern);
	bool putBack(const char pattern);
};

#endif//__EKH_SCRATCH_GRAPHICS_1_FILE_SERVICE__