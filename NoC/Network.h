#pragma once
#include "Router.h"
#include "Links.h"
#include "DopplerNode.h"

class Network
{
public:
	Network()
	{
		for (int i{}; i < m_routers.size(); ++i)
		{
			m_routers[i].m_routerID = { i / NETWORK_DIMENSION_X, i % NETWORK_DIMENSION_X };
		}
	};

	void runOneStep();

	void setUpConnections();
	void printRouterIDs();
	void mountNode(RouterID routerID, DopplerNode* node);
	void printNodeIDs();
	void viewMappingTable();
	void updateMappingTables();
	void terminateConnections();
	void updateEnable();

public:
	std::array<Router, ROUTER_NUMBER> m_routers{};
	Links m_links{};

private:
	std::vector<MappingTableLine> m_mappingTable{};
};