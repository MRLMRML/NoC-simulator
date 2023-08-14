#include "Port.h"

bool Port::areEmpty()
{
	return (outFlitBuffer.empty() && inFlitBuffer.empty());
}