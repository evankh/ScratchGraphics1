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

class auto_cstr {
	char* mData = nullptr;
	friend std::string operator+(std::string, auto_cstr);
public:
	auto_cstr(char* a = nullptr) {
		mData = a;
	};
	auto_cstr(const auto_cstr& other) {
		int len = strlen(other.mData);
		mData = new char[len + 1];
		memcpy(mData, other.mData, len + 1);
	};
	auto_cstr(auto_cstr&& other) {
		mData = other.mData;
		other.mData = nullptr;
	};
	~auto_cstr() {
		if (mData) delete[] mData;
	};
	operator std::string() const {
		return std::string(mData);
	};
	operator char*() const {
		return mData;
	};
};

#endif//__EKH_SCRATCH_GRAPHICS_1_FILE_SERVICE__