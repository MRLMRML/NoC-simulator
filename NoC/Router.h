#pragma once
#include "DataStructures.h"
#include "TerminalPort.h"
#include "RouterPort.h"
#include "CreditMaintainer.h"

class Router
{
public:
	Router()
	{
		initiateVirtualChannelPriority();
	}

	void runOneStep();

	// RC
	void computeRoute();
	void routeTerminalPort();
	void routeNorthPort();
	void routeSouthPort();
	void routeWestPort();
	void routeEastPort();

	// VA
	void initiateVirtualChannelPriority();
	void allocateVirtualChannel();
	void allocateTerminalPortVirtualChannel();
	void allocateNorthPortVirtualChannel();
	void allocateSouthPortVirtualChannel();
	void allocateWestPortVirtualChannel();
	void allocateEastPortVirtualChannel();
	void winVirtualChannelArbitration(const PortType port, const int virtualChannel);
	void activateVirtualChannel();
	void updateVirtualChannelPriority();

	// SA
	void allocateSwitch();

	void winSwitchArbitration(const PortType port);
	void updateSwitchPriority();

	// ST
	void traverseSwitch();

	//void compensateCycle();

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
	std::vector<VirtualChannelArbitrationRecorderLine> m_virtualChannelArbitrationRecorder{}; // record the winner of virtual channel arbitration; used in updateVirtualChannelPriority();
	
	// SA
	std::vector<PortType> m_switchArbitrationRecorder{}; // record the winner of switch arbitration; used in updateSwitchPriority();

	// ST
	
	//CreditMaintainer m_creditMaintainer{}; // ???

private:
};