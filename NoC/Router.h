#pragma once
#include "DataStructures.h"
#include "NorthPort.h"
#include "SouthPort.h"
#include "WestPort.h"
#include "EastPort.h"
#include "TerminalPort.h"

class Router
{
public:
	Router() = default;

	void runOneStep();

	void receiveFlit();

	void computeRoute();

	void allocateVirtualChannel();

	void allocateSwitchAndTraverseSwitch();

	void compensateCycle();

public:
	NorthPort m_northPort{};
	SouthPort m_southPort{};
	WestPort m_westPort{};
	EastPort m_eastPort{};
	TerminalPort m_terminalPort{};

	RouterID m_routerID{};
	int m_NID{ -1 }; // not every router has valid m_NID!
	//std::vector<MappingTableLine> m_mappingTable{};

private:
};