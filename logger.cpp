#include <stdarg.h>
#include <stdlib.h>
#include "logger.h"

Log * Log::instance = NULL;

Log::Log()
{
}

Log::~Log()
{

}

void Log::log(std::string log)
{
	printf(log.c_str());

}

void Log::clientlog(std::string log)
{
	
}