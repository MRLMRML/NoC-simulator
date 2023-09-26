#pragma once
#include "DataStructures.h"
#include "Port.h"

class DopplerNode
{
public:
	DopplerNode() = default;

	void runOneStep();

	void generatePacket();
	void viewPacket(const Packet& packet);
	void viewFlit(const Flit& flit);

	void sendPacket(const Packet packet);
	void dismantlePacket(const Packet packet);

	void assemblePacket();
	void receivePacket(const Packet packet);

public:
	Port m_port{ PortType::TerminalPort };
	int m_NID{}; // node ID of this DopplerNode

private:
	std::vector<Flit> m_flitReorderBuffer{}; // store the received flits; if received TailFlit/HeadTailFlit, lookup the buffer and form the packet 
};