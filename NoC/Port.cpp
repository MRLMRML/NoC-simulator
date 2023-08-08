#include "Port.h"

bool Port::isEmpty()
{
	return (outBuffer.empty() && inBuffer.empty());
}