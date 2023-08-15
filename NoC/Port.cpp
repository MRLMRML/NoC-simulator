#include "Port.h"

bool Port::isEmpty()
{
	return (outFlitBuffer.empty() && inFlitBuffer.empty());
}

Direction Port::computeRoute(const Direction port, Flit flit)
{

}
