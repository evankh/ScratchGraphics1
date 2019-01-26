#ifndef __EKH_SCRATCH_GRAPHICS_1_LOGGING_SERVICE__
#define __EKH_SCRATCH_GRAPHICS_1_LOGGING_SERVICE__

#include <iostream>
#include <fstream>
#include <string>
#include <cassert>

class LoggingService {
public:
	virtual void log(std::string message) = 0;
	virtual void error(std::string error, std::string info) = 0;
	virtual void warning(std::string warning, std::string info) = 0;
	virtual void badFileError(std::string filename) = 0;
	virtual void fileError(std::string filename, int line, std::string error, std::string info) = 0;
};

class NullLoggingService :public LoggingService {
	virtual void log(std::string message) {};
	virtual void error(std::string error, std::string info) {};
	virtual void warning(std::string warning, std::string info) {};
	virtual void badFileError(std::string filename) {};
	virtual void fileError(std::string filename, int line, std::string error, std::string info) {};
};

class ConsoleLoggingService :public LoggingService {
	virtual void log(std::string message) { std::cout << message << std::endl; };
	virtual void error(std::string error, std::string info) { std::cerr << "Error: " << error << ": " << info << std::endl; };
	virtual void warning(std::string warning, std::string info) { std::cerr << "Warning: " << warning << ": " << info << std::endl; };
	virtual void badFileError(std::string filename) { std::cerr << "Unable to open file: " << filename << std::endl; };
	virtual void fileError(std::string filename, int line, std::string error, std::string info) { std::cerr << "Error @ line " << line << " in file " << filename << ": " << error << ": " << info << std::endl; }
};

class FileLoggingService :public LoggingService {
private:
	std::ofstream fileout;
public:
	FileLoggingService(char* filename);
	virtual void log(std::string message) { fileout << message << std::endl; };
	virtual void error(std::string error, std::string info) { fileout << "Error: " << error << ": " << info << std::endl; };
	virtual void warning(std::string warning, std::string info) { fileout << "Warning: " << warning << ": " << info << std::endl; };
	virtual void badFileError(std::string filename) { fileout << "Unable to open file: " << filename << std::endl; };
	virtual void fileError(std::string filename, int line, std::string error, std::string info) { std::cerr << "Error @ line " << line << " in file " << filename << ": " << error << ": " << info << std::endl; }
};

#endif//__EKH_SCRATCH_GRAPHICS_1_LOGGING_SERVICE__