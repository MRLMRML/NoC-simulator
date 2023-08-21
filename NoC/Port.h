#pragma once
#include "DataStructures.h"

class Port
{
public:
	Port() = default;
	Port(const PortType portType) : m_portType{ portType } {}
	virtual ~Port() = default;

	// Input port
	std::deque<Flit> outFlitBuffer;
	std::deque<Credit> inCreditBuffer;

	// Output port
	std::deque<Flit> inFlitBuffer;
	std::deque<Credit> outCreditBuffer;

	bool isEmpty();

	void receiveFlit();
	virtual void computeRoute() {};

public:
	RouterID m_routerID{};
	PortType m_portType{PortType::Unselected};
	std::vector<MappingTableLine> m_mappingTable{};

private:
};