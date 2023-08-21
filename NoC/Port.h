#pragma once
#include "DataStructures.h"

class Port
{
public:
	Port() = default;
	Port(const PortType portType) : m_portType{ portType } {}
	virtual ~Port() = default;

	// Input port
	std::deque<Flit> m_outFlitBuffer;
	std::deque<Credit> m_inCreditBuffer;

	// Output port
	std::deque<Flit> m_inFlitBuffer;
	std::deque<Credit> m_outCreditBuffer;

	bool isEmpty();

	virtual void computeRoute() {};

public:
	RouterID m_routerID{};
	PortType m_portType{PortType::Unselected};
	std::vector<MappingTableLine> m_mappingTable{};

private:
};