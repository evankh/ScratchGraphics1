#include "Util/ServiceLocator.h"

ServiceLocator::~ServiceLocator() {
	delete loggingService;
}

LoggingService* ServiceLocator::loggingService = new NullLoggingService();

void ServiceLocator::provideLoggingService(LoggingService* service) {
	if (loggingService)
		delete loggingService;
	if (service)
		loggingService = service;
	else
		loggingService = new NullLoggingService();
}