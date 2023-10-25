#include "Log.h"

void log(std::string_view string)
{
	std::cout << string << std::endl;
}

void logDebug(std::string_view string)
{
#if defined (DEBUG)
	std::cout << string << std::endl;
#endif
}

void log(const DATA_PRECISION& value)
{
	std::cout << value << std::endl;
}

void log(const size_t& value)
{
	std::cout << value << std::endl;
}

void log(const int& value)
{
	std::cout << value << std::endl;
}

void log(const long& value)
{
	std::cout << value << std::endl;
}