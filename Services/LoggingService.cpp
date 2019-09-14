#include "Services/LoggingService.h"

FileLoggingService::FileLoggingService(char* filename) {
	fileout = std::ofstream(filename);
	assert(!fileout.fail());
}