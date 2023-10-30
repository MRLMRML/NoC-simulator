#pragma once
#include "DataStructures.h"
#include "TerminalPort.h"
#include "RouterPort.h"
#include "Crossbar.h"

class Router
{
public:
	Router()
	{
		initiateVirtualChannelPriority();
		initiateSwitchPriority();
	}

	void runOneStep();
	void receiveFlitAndCredit();

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
	void winVirtualChannelArbitration(const PortType port, const int virtualChannel, const int virtualChannelPriority);
	void loseVirtualChannelArbitration(const PortType port, const int virtualChannel, const int virtualChannelPriority);
	void activateVirtualChannel();
	void updateVirtualChannelPriority();
	
	// SA
	void initiateSwitchPriority();
	void allocateSwitch();
	void allocateTerminalPortSwitch();
	void allocateNorthPortSwitch();
	void allocateSouthPortSwitch();
	void allocateWestPortSwitch();
	void allocateEastPortSwitch();
	void winSwitchArbitration(const PortType port, const PortType outputPortSwitched, const int switchPriorityGlobal);
	void loseSwitchArbitration(const PortType port, const PortType outputPortSwitched, const int switchPriorityGlobal);
	void switchArbitration();
	void updateSwitchPriority();
	void getOneFlitOut();

	// ST
	void traverseSwitch();
	void setUpCrossbarConnections();
	void transmitCredits();
	void resetFields();
	void resetSAFields(const PortType port);
	void resetRCVAInputFields(const PortType port, const int virtualChannel);
	void resetRCVAOutputFields(const PortType port, const int virtualChannel); // when credits are full: A -> I

	// ST and SA in parallel
	void traverseThisSwitchThenAllocateNextSwitch();

	// Timing
	void synchronizeTriggerClocks();
	void synchronizeExecutionClocks();

	// Enable
	void updateEnable();

	// debug
	void viewData();
	void viewTerminalPortData();
	void viewNorthPortData();
	void viewSouthPortData();
	void viewWestPortData();
	void viewEastPortData();

public:
	RouterPort m_northPort{ PortType::NorthPort };
	RouterPort m_southPort{ PortType::SouthPort };
	RouterPort m_westPort{ PortType::WestPort };
	RouterPort m_eastPort{ PortType::EastPort };
	TerminalPort m_terminalPort{ PortType::TerminalPort };
	Crossbar m_crossbar{};

	// RC
	RouterID m_routerID{};
	int m_NID{ -1 }; // not every router has valid m_NID! Routers with node mounted have the NID of the node
	std::vector<MappingTableLine> m_mappingTable{};

	// VA
	std::vector<VirtualChannelArbitrationRecorderLine> m_virtualChannelArbitrationRecorder{}; // record the winner of virtual channel arbitration; used in updateVirtualChannelPriority();
	
	// SA
	std::vector<SwitchArbitrationRecorderLine> m_switchArbitrationRecorder{}; // record the winner of switch arbitration; used in updateSwitchPriority();

	// ST

private:
};