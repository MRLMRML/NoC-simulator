#include "Port.h"

bool Port::isEmpty()
{
	return (outFlitBuffer.empty() && inFlitBuffer.empty());
}

void Port::receiveFlit()
{

}
