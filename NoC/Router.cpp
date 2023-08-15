#include "Router.h"

void Router::runOneStep()
{
	// arbitration??
	m_northPort.runOneStep();
	m_southPort.runOneStep();
	m_westPort.runOneStep();
	m_eastPort.runOneStep();
	m_terminalPort.runOneStep();
}