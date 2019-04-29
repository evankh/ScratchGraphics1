#ifndef __EKH_SCRATCH_GRAPHICS_1_FILE_SERVICE__
#define __EKH_SCRATCH_GRAPHICS_1_FILE_SERVICE__

#include <fstream>
#include <string>

class FileException :public std::runtime_error {
public:
	FileException(std::string what_arg) :std::runtime_error(what_arg) {};
};

class FileNotFoundException :public FileException {
public:
	FileNotFoundException(std::string filename) :FileException(std::string("File not found: ") + filename) {};
};

class FileService {
private:
	std::ifstream in;
	std::string mPath;
	bool mBackwards;
public:
	FileService(const char* filename);
	FileService(std::string filename);
	~FileService();
	bool good() { return in.good(); };
	const std::string getPath() const { return mPath; };
	std::string getAll();
	bool extract(const char* pattern, void* target = nullptr);
	void restart();
	bool putBack(const char* pattern);
	bool putBack(const char pattern);
	void jumpToPosition(int position);
	void jumpToOffset(int offset);
	void reverseEndian();
};

#endif//__EKH_SCRATCH_GRAPHICS_1_FILE_SERVICE__