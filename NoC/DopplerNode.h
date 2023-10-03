#pragma once
#include "DataStructures.h"
#include "Port.h"

class DopplerNode
{
public:
	DopplerNode() = default;
	DopplerNode(const int NID) : m_NID{ NID } {}
	DopplerNode(const int NID, const bool isSilent) : m_NID{ NID }, m_isSilent{ isSilent } {}
	DopplerNode(const int NID, const Packet& packetGenerated) : m_NID{ NID }, m_packetGenerated{ packetGenerated } 
	{
		m_isSilent = false;
	}
	
	void runOneStep();

	void viewPacket(const Packet& packet);
	void viewFlit(const Flit& flit);

	void injectTraffic();
	void generatePacket();
	void dismantlePacket();
	void recordInputTime();
	void sendFlit();

	void collectTraffic();
	bool receiveFlit();
	void assemblePacket();
	void recordOutputTime();

public:
	Port m_port{ PortType::TerminalPort };
	int m_NID{}; // node ID of this DopplerNode

private:
	bool m_isSilent{ true };
	int m_injectTimes{ 1 };
	Packet m_packetGenerated{};
	Packet m_packetReceived{};
	std::deque<Flit> m_sourceQueue{};
	std::vector<Flit> m_flitReorderBuffer{}; // store the received flits; if received TailFlit/HeadTailFlit, lookup the buffer and form the packet 
};