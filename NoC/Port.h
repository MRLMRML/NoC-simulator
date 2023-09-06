#pragma once
#include "DataStructures.h"

class Port
{
public:
	Port() = default;
	Port(const PortType portType) : m_portType{ portType } {}
	virtual ~Port() = default;

	bool isEmpty();

	// Input port
	std::deque<Flit> m_inFlitBuffer;
	std::deque<Credit> m_outCreditBuffer;
	std::deque<Flit> m_crossbarInputBuffer;

	// Output port
	std::deque<Flit> m_outFlitBuffer;
	std::deque<Credit> m_inCreditBuffer;

	// port type
	PortType m_portType{};

	// SA
	int m_switchPriorityGlobal{};
	PortType m_inputPortSwitched{}; // Unselected (default)

private:
};