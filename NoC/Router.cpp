#include "Router.h"

void Router::runOneStep()
{
	receiveFlit();
	computeRoute();
	allocateVirtualChannel();
	allocateSwitchAndTraverseSwitch();
}

void Router::receiveFlit()
{
	m_northPort.receiveFlit();
	m_southPort.receiveFlit();
	m_westPort.receiveFlit();
	m_eastPort.receiveFlit();
}

void Router::computeRoute()
{
	m_northPort.computeRoute();
	m_southPort.computeRoute();
	m_westPort.computeRoute();
	m_eastPort.computeRoute();
	m_terminalPort.computeRoute();
}