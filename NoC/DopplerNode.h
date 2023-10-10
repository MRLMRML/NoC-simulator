#pragma once
#include "DataStructures.h"
#include "Port.h"
#include "Clock.h"
#include <fstream>
#include <sstream>
#include <cstdio>

extern std::string g_dataFolderPath;
extern std::string g_packetRecordPath;

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
	void readPacket();
	void dismantlePacket();
	void recordInputTime(const int packetInputTime);
	void sendFlit();

	void collectTraffic();
	bool receiveFlit();
	void assemblePacket();
	void recordOutputTime(const int packetInputTime);

public:
	Port m_port{ PortType::TerminalPort };
	int m_NID{}; // node ID of this DopplerNode
	Clock m_localClock{};

private:
	bool m_isSilent{ true };
	int m_packetIDTracker{ 0 };
	Packet m_packetGenerated{};
	Packet m_packetReceived{};
	std::deque<Flit> m_sourceQueue{};
	std::vector<Flit> m_flitReorderBuffer{}; // store the received flits; if received TailFlit/HeadTailFlit, lookup the buffer and form the packet 
};