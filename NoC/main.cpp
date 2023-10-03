#include "Network.h"

int main()
{
	// doppler packets
	//Packet packet0{ 1, 0 };

	// create doppler nodes
	DopplerNode* node0{ new DopplerNode{0, false} };
	DopplerNode* node1{ new DopplerNode{1} };
	//DopplerNode* node2{ new DopplerNode{2} };
	//DopplerNode* node3{ new DopplerNode{3, packet3} };
	//DopplerNode* node4{ new DopplerNode{4} };
	//DopplerNode* node5{ new DopplerNode{5, packet5} };
	//DopplerNode* node6{ new DopplerNode{6} };
	//DopplerNode* node7{ new DopplerNode{7, packet7} };
	//DopplerNode* node8{ new DopplerNode{8} };

	// create network
	Network* network{ new Network{} };
	network->setUpConnections();
	network->printRouterIDs();

	// mount nodes on the network
	network->mountNode({ 0, 0 }, node0);
	network->mountNode({ 0, 1 }, node1);
	//network->mountNode({ 0, 2 }, node2);
	//network->mountNode({ 1, 0 }, node3);
	//network->mountNode({ 1, 1 }, node4);
	//network->mountNode({ 1, 2 }, node5);
	//network->mountNode({ 2, 0 }, node6);
	//network->mountNode({ 2, 1 }, node7);
	//network->mountNode({ 2, 2 }, node8);

	network->printNodeIDs();
	network->viewMappingTable();
	network->updateMappingTables();

	//std::vector<DopplerNode*> dopplerNodes;
	//dopplerNodes.push_back(node0);
	//dopplerNodes.push_back(node1);
	//dopplerNodes.push_back(node3);
	//dopplerNodes.push_back(node4);
	//dopplerNodes.push_back(node5);
	//dopplerNodes.push_back(node7);

	//while (true)
	//{
	//	for (auto& node : dopplerNodes)
	//	{
	//		node->runOneStep();
	//	}
	//	network->runOneStep();
	//}

	node0->runOneStep();
	node1->runOneStep();
	network->m_links.runOneStep();
	network->m_routers.at(0).runOneStep();
	network->m_routers.at(1).runOneStep();
	
	node0->runOneStep();
	node1->runOneStep();
	network->m_links.runOneStep();
	network->m_routers.at(0).runOneStep();
	network->m_routers.at(1).runOneStep();
	
	node0->runOneStep();
	node1->runOneStep();
	network->m_links.runOneStep();
	network->m_routers.at(0).runOneStep();
	network->m_routers.at(1).runOneStep();

	node0->runOneStep();
	node1->runOneStep();
	network->m_links.runOneStep();
	network->m_routers.at(0).runOneStep();
	network->m_routers.at(1).runOneStep();
	
	node0->runOneStep();
	node1->runOneStep();
	network->m_links.runOneStep();
	network->m_routers.at(0).runOneStep();
	network->m_routers.at(1).runOneStep();
	
	node0->runOneStep();
	node1->runOneStep();
	network->m_links.runOneStep();
	network->m_routers.at(0).runOneStep();
	network->m_routers.at(1).runOneStep();

	node0->runOneStep();
	node1->runOneStep();
	network->m_links.runOneStep();
	network->m_routers.at(0).runOneStep();
	network->m_routers.at(1).runOneStep();

	//node0->runOneStep();
	//network->m_links.runOneStep();
	//network->m_routers.front().viewData();

	//network->m_routers.front().runOneStep();
	//network->m_routers.front().viewData();

	//network->m_routers.front().runOneStep();
	//network->m_routers.front().viewData();
	
	//// testing
	//node0->runOneStep();
	//while(true)
	//	network->runOneStep();
}