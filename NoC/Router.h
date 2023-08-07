#pragma once

class Router
{
public:
	Router() = default;

	void runOneStep();

public:
	Port m_northPort{};
	Port m_southPort{};
	Port m_westPort{};
	Port m_eastPort{};
	Port m_terminalPort{};

	RouterID m_routerID{};
	int m_NID{ -1 }; // not every router has valid m_NID!
	std::vector<MappingTableLine> m_mappingTable{};
private:
};