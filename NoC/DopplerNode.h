#pragma once
#include "DataStructures.h"
#include "Port.h"

class DopplerNode
{
public:
	DopplerNode() = default;

	void runOneStep();

	void viewPacket(const Packet& packet);
	void viewFlit(const Flit& flit);

	void injectTraffic();
	void generatePacket();
	void dismantlePacket();
	void recordInputTime();
	void sendFlit();

	void collectTraffic();
	void receiveFlit();
	void assemblePacket();
	void recordOutputTime();

public:
	Port m_port{ PortType::TerminalPort };
	int m_NID{}; // node ID of this DopplerNode

private:
	Packet m_packetGenerated{};
	Packet m_packetReceived{};
	std::deque<Flit> m_sourceQueue{};
	std::vector<Flit> m_flitReorderBuffer{}; // store the received flits; if received TailFlit/HeadTailFlit, lookup the buffer and form the packet 
};