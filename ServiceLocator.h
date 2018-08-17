#ifndef __EKH_SCRATCH_GRAPHICS_1_SERVICE_LOCATOR__
#define __EKH_SCRATCH_GRAPHICS_1_SERVICE_LOCATOR__

#include "Services\FileService.h"
#include "Services\LoggingService.h"

class ServiceLocator {
private:
	static LoggingService* loggingService;
public:
	~ServiceLocator();
	static void provideLoggingService(LoggingService* service);
	static LoggingService& getLoggingService() { return *loggingService; };
	static FileService& getFileService(const char* filename) { return *new FileService(filename); };
	static FileService& getFileService(std::string filename) { return *new FileService(filename.data()); };
};

#endif//__EKH_SCRATCH_GRAPHICS_1_SERVICE_LOCATOR__