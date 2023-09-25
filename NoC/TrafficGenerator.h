#pragma once
#include "DataStructures.h"
#include "TerminalPort.h"

class TrafficGenerator
{
public:
	TrafficGenerator() = default;

	TerminalPort m_terminalPort{ PortType::TerminalPort };

private:
};