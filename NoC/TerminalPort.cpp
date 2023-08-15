#include "TerminalPort.h"

void TerminalPort::runOneStep()
{
	if (!inFlitBuffer.empty())
	{
		Flit flit{ inFlitBuffer.front() };
		inFlitBuffer.pop_front();

		if (flit.flitType == FlitType::HeadFlit)
		{

		}
	}

}