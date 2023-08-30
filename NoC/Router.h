#pragma once
#include "DataStructures.h"
#include "TerminalPort.h"
#include "RouterPort.h"
#include "CreditMaintainer.h"
//#include "VirtualChannelPriorityTable.h"

class Router
{
public:
	Router()
	{
		initiatePriorities();
	}

	void runOneStep();

	// RC
	void computeRoute();
	void routeNorthPort();
	void routeSouthPort();
	void routeWestPort();
	void routeEastPort();
	void routeTerminalPort();

	// VA
	void initiatePriorities();
	void winArbitration(const PortType port, const int virtualChannel);
	void allocateVirtualChannel();
	void activateVirtualChannel();
	void updatePriority();

	// SA/ST
	void allocateSwitchAndTraverseSwitch();

	void compensateCycle();

public:
	RouterPort m_northPort{};
	RouterPort m_southPort{};
	RouterPort m_westPort{};
	RouterPort m_eastPort{};
	TerminalPort m_terminalPort{};

	// RC
	RouterID m_routerID{};
	int m_NID{ -1 }; // not every router has valid m_NID!
	std::vector<MappingTableLine> m_mappingTable{};

	// VA
	std::vector<ArbitrationRecorderLine> m_arbitrationRecorder{}; // record the winner of arbitration; used in updatePriorities();
	
	// SA/ST
	CreditMaintainer m_creditMaintainer{}; // ???

private:
};