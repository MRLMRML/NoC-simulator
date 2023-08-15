#pragma once
#include "DataStructures.h"
#include "RouterPort.h"

class Router
{
public:
	Router() = default;

	void runOneStep();

	

public:
	RouterPort m_northPort{};
	RouterPort m_southPort{};
	RouterPort m_westPort{};
	RouterPort m_eastPort{};
	Port m_terminalPort{};

	RouterID m_routerID{};
	int m_NID{ -1 }; // not every router has valid m_NID!
	std::vector<MappingTableLine> m_mappingTable{};

private:
};