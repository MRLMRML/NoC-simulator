#include "Network.h"

void Network::setUpConnections()
{
#if defined(MESH)
	for (size_t j{}; j < NETWORK_DIMENSION_Y; ++j)
	{
		for (size_t i{}; i < NETWORK_DIMENSION_X; ++i)
		{
			if (i != NETWORK_DIMENSION_X - 1)
				m_links.setUpConnection(m_routers[i + j * NETWORK_DIMENSION_Y].m_eastPort, m_routers[(i + 1) + j * NETWORK_DIMENSION_Y].m_westPort);

			if (j != NETWORK_DIMENSION_Y - 1)
				m_links.setUpConnection(m_routers[i + j * NETWORK_DIMENSION_Y].m_southPort, m_routers[i + (j + 1) * NETWORK_DIMENSION_Y].m_northPort);
		}
	}
#endif

#if defined(TORUS)
	for (size_t j{}; j < NETWORK_DIMENSION_Y; ++j)
	{
		for (size_t i{}; i < NETWORK_DIMENSION_X; ++i)
		{
			if (i == NETWORK_DIMENSION_X - 1)
				m_links.setUpConnection(m_routers[i + j * NETWORK_DIMENSION_Y].m_eastPort, m_routers[0 + j * NETWORK_DIMENSION_Y].m_westPort);
			else
				m_links.setUpConnection(m_routers[i + j * NETWORK_DIMENSION_Y].m_eastPort, m_routers[(i + 1) + j * NETWORK_DIMENSION_Y].m_westPort);

			if (j == NETWORK_DIMENSION_Y - 1)
				m_links.setUpConnection(m_routers[i + j * NETWORK_DIMENSION_Y].m_southPort, m_routers[i + 0 * NETWORK_DIMENSION_Y].m_northPort);
			else
				m_links.setUpConnection(m_routers[i + j * NETWORK_DIMENSION_Y].m_southPort, m_routers[i + (j + 1) * NETWORK_DIMENSION_Y].m_northPort);
		}
	}
#endif
}

void Network::printRouterIDs()
{
	for (size_t j{}; j < NETWORK_DIMENSION_Y; ++j)
	{
		for (size_t i{}; i < NETWORK_DIMENSION_X; ++i)
		{
			std::cout << m_routers[i + j * NETWORK_DIMENSION_Y].m_routerID;
		}
		std::cout << std::endl;
	}
}

void Network::mountNode(const RouterID& routerID, const Node* node)
{
	m_links.setUpConnection(m_routers[static_cast<std::array<Router, 16Ui64>::size_type>(routerID.y) * NETWORK_DIMENSION_X + routerID.x].m_terminalPort, node->m_NI->m_port);
	m_routers[static_cast<std::array<Router, 16Ui64>::size_type>(routerID.y) * NETWORK_DIMENSION_X + routerID.x].m_NID = node->m_NI->m_NID;
	m_mappingTable.push_back({ routerID, node->m_NI->m_NID });
}

void Network::printNodeIDs()
{
	for (size_t j{}; j < NETWORK_DIMENSION_Y; ++j)
	{
		for (size_t i{}; i < NETWORK_DIMENSION_X; ++i)
		{
			if (m_routers[i + j * NETWORK_DIMENSION_Y].m_NID != -1)
				std::cout << " " << m_routers[i + j * NETWORK_DIMENSION_Y].m_NID;
			else
				std::cout << " -";
		}
		std::cout << std::endl;
	}
}

void Network::viewMappingTable()
{
	for (auto& mappingTableLine : m_mappingTable)
	{
		std::cout << mappingTableLine.routerID << "| " << mappingTableLine.NID << std::endl;
	}
}

void Network::updateMappingTables()
{
	for (auto& router : m_routers)
	{
		router.m_mappingTable = m_mappingTable;
	}
}

void Network::terminateConnections()
{
	m_links.terminateAllConnections();
}

bool Network::isQuiet()
{
	return m_links.areEmpty();
}

void Network::runOneStep()
{
	if (globalCycle >= m_networkCycle)
	{
		m_links.runOneStep();
		for (auto& router : m_routers)
		{
			router.runOneStep();
		}

		m_networkCycle += LINKS_CYCLES + ROUTER_CYCLES;
	}
}