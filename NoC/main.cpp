#include "Network.h"

// global variables
std::string g_dataFolderPath{ "C:\\Users\\Hubiao\\source\\repos\\NoC\\NoC\\Data\\" };
std::string g_packetRecordPath{ "PacketRecord.csv" };

void generateTraffic()
{
	std::ofstream writePacketRecord(g_dataFolderPath + g_packetRecordPath, std::ios::out);
	writePacketRecord 
		<< "Source" << ","
		<< "Packet_ID" << ","
		<< "Destination" << ","
		<< "Status" << ","
		<< "Input_time" << ","
		<< "Output_time" << ","
		<< std::endl;

	writePacketRecord 
		<< "1" << ","
		<< "0" << ","
		<< "7" << ","
		<< "intact" << ","
		<< "-" << ","
		<< "-" << ","
		<< std::endl;
	writePacketRecord
		<< "1" << ","
		<< "1" << ","
		<< "7" << ","
		<< "intact" << ","
		<< "-" << ","
		<< "-" << ","
		<< std::endl;

	writePacketRecord 
		<< "3" << ","
		<< "0" << ","
		<< "7" << ","
		<< "intact" << ","
		<< "-" << ","
		<< "-" << ","
		<< std::endl;
	writePacketRecord
		<< "3" << ","
		<< "1" << ","
		<< "7" << ","
		<< "intact" << ","
		<< "-" << ","
		<< "-" << ","
		<< std::endl;

	writePacketRecord
		<< "5" << ","
		<< "0" << ","
		<< "7" << ","
		<< "intact" << ","
		<< "-" << ","
		<< "-" << ","
		<< std::endl;
	writePacketRecord
		<< "5" << ","
		<< "1" << ","
		<< "7" << ","
		<< "intact" << ","
		<< "-" << ","
		<< "-" << ","
		<< std::endl;

	writePacketRecord.close();
}

int main()
{
	// generate traffic
	generateTraffic();

	// timing
	Clock globalClock{};

	// doppler packets
	//Packet packet0{ 1, 0 };

	// create doppler nodes
	DopplerNode* node0{ new DopplerNode{0} };
	DopplerNode* node1{ new DopplerNode{1, false} };
	DopplerNode* node2{ new DopplerNode{2} };
	DopplerNode* node3{ new DopplerNode{3, false} };
	DopplerNode* node4{ new DopplerNode{4} };
	DopplerNode* node5{ new DopplerNode{5, false} };
	DopplerNode* node6{ new DopplerNode{6} };
	DopplerNode* node7{ new DopplerNode{7, false} };
	DopplerNode* node8{ new DopplerNode{8} };


	// create network
	Network* network{ new Network{} };
	network->setUpConnections();
	network->printRouterIDs();

	// mount nodes on the network
	network->mountNode({ 0, 0 }, node0);
	network->mountNode({ 0, 1 }, node1);
	network->mountNode({ 0, 2 }, node2);
	network->mountNode({ 1, 0 }, node3);
	network->mountNode({ 1, 1 }, node4);
	network->mountNode({ 1, 2 }, node5);
	network->mountNode({ 2, 0 }, node6);
	network->mountNode({ 2, 1 }, node7);
	network->mountNode({ 2, 2 }, node8);

	network->printNodeIDs();
	network->viewMappingTable();
	network->updateMappingTables();

	std::vector<DopplerNode*> dopplerNodes;
	dopplerNodes.push_back(node0);
	dopplerNodes.push_back(node1);
	dopplerNodes.push_back(node2);
	dopplerNodes.push_back(node3);
	dopplerNodes.push_back(node4);
	dopplerNodes.push_back(node5);
	dopplerNodes.push_back(node6);
	dopplerNodes.push_back(node7);
	dopplerNodes.push_back(node8);

	//for (int i{}; i < WARMUP_CYCLES; ++i)
	//{
	//	for (auto& node : dopplerNodes)
	//	{
	//		node->runOneStep();
	//	}
	//	network->runOneStep();
	//	globalClock.tickGlobalClock();
	//}

	for (int i{}; i < MEASUREMENT_CYCLES; ++i)
	{
		// update enable signals
		network->updateEnable();
		for (auto& node : dopplerNodes)
		{
			node->updateEnable();
		}

		for (auto& node : dopplerNodes)
		{
			node->runOneStep();
		}
		network->runOneStep();
		globalClock.tickGlobalClock();
	}

	//for (int i{}; i < DRAIN_CYCLES; ++i)
	//{
	//	for (auto& node : dopplerNodes)
	//	{
	//		node->runOneStep();
	//	}
	//	network->runOneStep();
	//	globalClock.tickGlobalClock();
	//}
	
	//network->updateEnable();
	//node1->updateEnable();
	//node3->updateEnable();
	//node4->updateEnable();
	//node5->updateEnable();
	//node7->updateEnable();
	//node1->runOneStep();
	//node3->runOneStep();
	//node4->runOneStep();
	//node5->runOneStep();
	//node7->runOneStep();
	//network->m_links.runOneStep();
	//network->m_routers.at(1).runOneStep();
	//network->m_routers.at(3).runOneStep();
	//network->m_routers.at(4).runOneStep();
	//network->m_routers.at(5).runOneStep();
	//network->m_routers.at(7).runOneStep();
	//globalClock.tickGlobalClock();

	//network->updateEnable();
	//node1->updateEnable();
	//node3->updateEnable();
	//node4->updateEnable();
	//node5->updateEnable();
	//node7->updateEnable();
	//node1->runOneStep();
	//node3->runOneStep();
	//node4->runOneStep();
	//node5->runOneStep();
	//node7->runOneStep();
	//network->m_links.runOneStep();
	//network->m_routers.at(1).runOneStep();
	//network->m_routers.at(3).runOneStep();
	//network->m_routers.at(4).runOneStep();
	//network->m_routers.at(5).runOneStep();
	//network->m_routers.at(7).runOneStep();
	//globalClock.tickGlobalClock();

	//network->updateEnable();
	//node1->updateEnable();
	//node3->updateEnable();
	//node4->updateEnable();
	//node5->updateEnable();
	//node7->updateEnable();
	//node1->runOneStep();
	//node3->runOneStep();
	//node4->runOneStep();
	//node5->runOneStep();
	//node7->runOneStep();
	//network->m_links.runOneStep();
	//network->m_routers.at(1).runOneStep();
	//network->m_routers.at(3).runOneStep();
	//network->m_routers.at(4).runOneStep();
	//network->m_routers.at(5).runOneStep();
	//network->m_routers.at(7).runOneStep();
	//globalClock.tickGlobalClock();

	//network->updateEnable();
	//node1->updateEnable();
	//node3->updateEnable();
	//node4->updateEnable();
	//node5->updateEnable();
	//node7->updateEnable();
	//node1->runOneStep();
	//node3->runOneStep();
	//node4->runOneStep();
	//node5->runOneStep();
	//node7->runOneStep();
	//network->m_links.runOneStep();
	//network->m_routers.at(1).runOneStep();
	//network->m_routers.at(3).runOneStep();
	//network->m_routers.at(4).runOneStep();
	//network->m_routers.at(5).runOneStep();
	//network->m_routers.at(7).runOneStep();
	//globalClock.tickGlobalClock();

	//network->updateEnable();
	//node1->updateEnable();
	//node3->updateEnable();
	//node4->updateEnable();
	//node5->updateEnable();
	//node7->updateEnable();
	//node1->runOneStep();
	//node3->runOneStep();
	//node4->runOneStep();
	//node5->runOneStep();
	//node7->runOneStep();
	//network->m_links.runOneStep();
	//network->m_routers.at(1).runOneStep();
	//network->m_routers.at(3).runOneStep();
	//network->m_routers.at(4).runOneStep();
	//network->m_routers.at(5).runOneStep();
	//network->m_routers.at(7).runOneStep();
	//globalClock.tickGlobalClock();

	//network->updateEnable();
	//node1->updateEnable();
	//node3->updateEnable();
	//node4->updateEnable();
	//node5->updateEnable();
	//node7->updateEnable();
	//node1->runOneStep();
	//node3->runOneStep();
	//node4->runOneStep();
	//node5->runOneStep();
	//node7->runOneStep();
	//network->m_links.runOneStep();
	//network->m_routers.at(1).runOneStep();
	//network->m_routers.at(3).runOneStep();
	//network->m_routers.at(4).runOneStep();
	//network->m_routers.at(5).runOneStep();
	//network->m_routers.at(7).runOneStep();
	//globalClock.tickGlobalClock();

	//network->updateEnable();
	//node1->updateEnable();
	//node3->updateEnable();
	//node4->updateEnable();
	//node5->updateEnable();
	//node7->updateEnable();
	//node1->runOneStep();
	//node3->runOneStep();
	//node4->runOneStep();
	//node5->runOneStep();
	//node7->runOneStep();
	//network->m_links.runOneStep();
	//network->m_routers.at(1).runOneStep();
	//network->m_routers.at(3).runOneStep();
	//network->m_routers.at(4).runOneStep();
	//network->m_routers.at(5).runOneStep();
	//network->m_routers.at(7).runOneStep();
	//globalClock.tickGlobalClock();

	//network->updateEnable();
	//node1->updateEnable();
	//node3->updateEnable();
	//node4->updateEnable();
	//node5->updateEnable();
	//node7->updateEnable();
	//node1->runOneStep();
	//node3->runOneStep();
	//node4->runOneStep();
	//node5->runOneStep();
	//node7->runOneStep();
	//network->m_links.runOneStep();
	//network->m_routers.at(1).runOneStep();
	//network->m_routers.at(3).runOneStep();
	//network->m_routers.at(4).runOneStep();
	//network->m_routers.at(5).runOneStep();
	//network->m_routers.at(7).runOneStep();
	//globalClock.tickGlobalClock();

	//network->updateEnable();
	//node1->updateEnable();
	//node3->updateEnable();
	//node4->updateEnable();
	//node5->updateEnable();
	//node7->updateEnable();
	//node1->runOneStep();
	//node3->runOneStep();
	//node4->runOneStep();
	//node5->runOneStep();
	//node7->runOneStep();
	//network->m_links.runOneStep();
	//network->m_routers.at(1).runOneStep();
	//network->m_routers.at(3).runOneStep();
	//network->m_routers.at(4).runOneStep();
	//network->m_routers.at(5).runOneStep();
	//network->m_routers.at(7).runOneStep();
	//globalClock.tickGlobalClock();

	//network->updateEnable();
	//node1->updateEnable();
	//node3->updateEnable();
	//node4->updateEnable();
	//node5->updateEnable();
	//node7->updateEnable();
	//node1->runOneStep();
	//node3->runOneStep();
	//node4->runOneStep();
	//node5->runOneStep();
	//node7->runOneStep();
	//network->m_links.runOneStep();
	//network->m_routers.at(1).runOneStep();
	//network->m_routers.at(3).runOneStep();
	//network->m_routers.at(4).runOneStep();
	//network->m_routers.at(5).runOneStep();
	//network->m_routers.at(7).runOneStep();
	//globalClock.tickGlobalClock();

	//network->updateEnable();
	//node1->updateEnable();
	//node3->updateEnable();
	//node4->updateEnable();
	//node5->updateEnable();
	//node7->updateEnable();
	//node1->runOneStep();
	//node3->runOneStep();
	//node4->runOneStep();
	//node5->runOneStep();
	//node7->runOneStep();
	//network->m_links.runOneStep();
	//network->m_routers.at(1).runOneStep();
	//network->m_routers.at(3).runOneStep();
	//network->m_routers.at(4).runOneStep();
	//network->m_routers.at(5).runOneStep();
	//network->m_routers.at(7).runOneStep();
	//globalClock.tickGlobalClock();

	//network->updateEnable();
	//node1->updateEnable();
	//node3->updateEnable();
	//node4->updateEnable();
	//node5->updateEnable();
	//node7->updateEnable();
	//node1->runOneStep();
	//node3->runOneStep();
	//node4->runOneStep();
	//node5->runOneStep();
	//node7->runOneStep();
	//network->m_links.runOneStep();
	//network->m_routers.at(1).runOneStep();
	//network->m_routers.at(3).runOneStep();
	//network->m_routers.at(4).runOneStep();
	//network->m_routers.at(5).runOneStep();
	//network->m_routers.at(7).runOneStep();
	//globalClock.tickGlobalClock();

	//network->updateEnable();
	//node1->updateEnable();
	//node3->updateEnable();
	//node4->updateEnable();
	//node5->updateEnable();
	//node7->updateEnable();
	//node1->runOneStep();
	//node3->runOneStep();
	//node4->runOneStep();
	//node5->runOneStep();
	//node7->runOneStep();
	//network->m_links.runOneStep();
	//network->m_routers.at(1).runOneStep();
	//network->m_routers.at(3).runOneStep();
	//network->m_routers.at(4).runOneStep();
	//network->m_routers.at(5).runOneStep();
	//network->m_routers.at(7).runOneStep();
	//globalClock.tickGlobalClock();

	//network->updateEnable();
	//node1->updateEnable();
	//node3->updateEnable();
	//node4->updateEnable();
	//node5->updateEnable();
	//node7->updateEnable();
	//node1->runOneStep();
	//node3->runOneStep();
	//node4->runOneStep();
	//node5->runOneStep();
	//node7->runOneStep();
	//network->m_links.runOneStep();
	//network->m_routers.at(1).runOneStep();
	//network->m_routers.at(3).runOneStep();
	//network->m_routers.at(4).runOneStep();
	//network->m_routers.at(5).runOneStep();
	//network->m_routers.at(7).runOneStep();
	//globalClock.tickGlobalClock();

	//network->updateEnable();
	//node1->updateEnable();
	//node3->updateEnable();
	//node4->updateEnable();
	//node5->updateEnable();
	//node7->updateEnable();
	//node1->runOneStep();
	//node3->runOneStep();
	//node4->runOneStep();
	//node5->runOneStep();
	//node7->runOneStep();
	//network->m_links.runOneStep();
	//network->m_routers.at(1).runOneStep();
	//network->m_routers.at(3).runOneStep();
	//network->m_routers.at(4).runOneStep();
	//network->m_routers.at(5).runOneStep();
	//network->m_routers.at(7).runOneStep();
	//globalClock.tickGlobalClock();

	//network->updateEnable();
	//node1->updateEnable();
	//node3->updateEnable();
	//node4->updateEnable();
	//node5->updateEnable();
	//node7->updateEnable();
	//node1->runOneStep();
	//node3->runOneStep();
	//node4->runOneStep();
	//node5->runOneStep();
	//node7->runOneStep();
	//network->m_links.runOneStep();
	//network->m_routers.at(1).runOneStep();
	//network->m_routers.at(3).runOneStep();
	//network->m_routers.at(4).runOneStep();
	//network->m_routers.at(5).runOneStep();
	//network->m_routers.at(7).runOneStep();
	//globalClock.tickGlobalClock();

	//network->updateEnable();
	//node1->updateEnable();
	//node3->updateEnable();
	//node4->updateEnable();
	//node5->updateEnable();
	//node7->updateEnable();
	//node1->runOneStep();
	//node3->runOneStep();
	//node4->runOneStep();
	//node5->runOneStep();
	//node7->runOneStep();
	//network->m_links.runOneStep();
	//network->m_routers.at(1).runOneStep();
	//network->m_routers.at(3).runOneStep();
	//network->m_routers.at(4).runOneStep();
	//network->m_routers.at(5).runOneStep();
	//network->m_routers.at(7).runOneStep();
	//globalClock.tickGlobalClock();

	//network->updateEnable();
	//node1->updateEnable();
	//node3->updateEnable();
	//node4->updateEnable();
	//node5->updateEnable();
	//node7->updateEnable();
	//node1->runOneStep();
	//node3->runOneStep();
	//node4->runOneStep();
	//node5->runOneStep();
	//node7->runOneStep();
	//network->m_links.runOneStep();
	//network->m_routers.at(1).runOneStep();
	//network->m_routers.at(3).runOneStep();
	//network->m_routers.at(4).runOneStep();
	//network->m_routers.at(5).runOneStep();
	//network->m_routers.at(7).runOneStep();
	//globalClock.tickGlobalClock();

	//network->updateEnable();
	//node1->updateEnable();
	//node3->updateEnable();
	//node4->updateEnable();
	//node5->updateEnable();
	//node7->updateEnable();
	//node1->runOneStep();
	//node3->runOneStep();
	//node4->runOneStep();
	//node5->runOneStep();
	//node7->runOneStep();
	//network->m_links.runOneStep();
	//network->m_routers.at(1).runOneStep();
	//network->m_routers.at(3).runOneStep();
	//network->m_routers.at(4).runOneStep();
	//network->m_routers.at(5).runOneStep();
	//network->m_routers.at(7).runOneStep();
	//globalClock.tickGlobalClock();

	//network->updateEnable();
	//node1->updateEnable();
	//node3->updateEnable();
	//node4->updateEnable();
	//node5->updateEnable();
	//node7->updateEnable();
	//node1->runOneStep();
	//node3->runOneStep();
	//node4->runOneStep();
	//node5->runOneStep();
	//node7->runOneStep();
	//network->m_links.runOneStep();
	//network->m_routers.at(1).runOneStep();
	//network->m_routers.at(3).runOneStep();
	//network->m_routers.at(4).runOneStep();
	//network->m_routers.at(5).runOneStep();
	//network->m_routers.at(7).runOneStep();
	//globalClock.tickGlobalClock();

	//network->updateEnable();
	//node1->updateEnable();
	//node3->updateEnable();
	//node4->updateEnable();
	//node5->updateEnable();
	//node7->updateEnable();
	//node1->runOneStep();
	//node3->runOneStep();
	//node4->runOneStep();
	//node5->runOneStep();
	//node7->runOneStep();
	//network->m_links.runOneStep();
	//network->m_routers.at(1).runOneStep();
	//network->m_routers.at(3).runOneStep();
	//network->m_routers.at(4).runOneStep();
	//network->m_routers.at(5).runOneStep();
	//network->m_routers.at(7).runOneStep();
	//globalClock.tickGlobalClock();

	//network->updateEnable();
	//node1->updateEnable();
	//node3->updateEnable();
	//node4->updateEnable();
	//node5->updateEnable();
	//node7->updateEnable();
	//node1->runOneStep();
	//node3->runOneStep();
	//node4->runOneStep();
	//node5->runOneStep();
	//node7->runOneStep();
	//network->m_links.runOneStep();
	//network->m_routers.at(1).runOneStep();
	//network->m_routers.at(3).runOneStep();
	//network->m_routers.at(4).runOneStep();
	//network->m_routers.at(5).runOneStep();
	//network->m_routers.at(7).runOneStep();
	//globalClock.tickGlobalClock();

	//network->updateEnable();
	//node1->updateEnable();
	//node3->updateEnable();
	//node4->updateEnable();
	//node5->updateEnable();
	//node7->updateEnable();
	//node1->runOneStep();
	//node3->runOneStep();
	//node4->runOneStep();
	//node5->runOneStep();
	//node7->runOneStep();
	//network->m_links.runOneStep();
	//network->m_routers.at(1).runOneStep();
	//network->m_routers.at(3).runOneStep();
	//network->m_routers.at(4).runOneStep();
	//network->m_routers.at(5).runOneStep();
	//network->m_routers.at(7).runOneStep();
	//globalClock.tickGlobalClock();

	//network->updateEnable();
	//node1->updateEnable();
	//node3->updateEnable();
	//node4->updateEnable();
	//node5->updateEnable();
	//node7->updateEnable();
	//node1->runOneStep();
	//node3->runOneStep();
	//node4->runOneStep();
	//node5->runOneStep();
	//node7->runOneStep();
	//network->m_links.runOneStep();
	//network->m_routers.at(1).runOneStep();
	//network->m_routers.at(3).runOneStep();
	//network->m_routers.at(4).runOneStep();
	//network->m_routers.at(5).runOneStep();
	//network->m_routers.at(7).runOneStep();
	//globalClock.tickGlobalClock();

	//network->updateEnable();
	//node1->updateEnable();
	//node3->updateEnable();
	//node4->updateEnable();
	//node5->updateEnable();
	//node7->updateEnable();
	//node1->runOneStep();
	//node3->runOneStep();
	//node4->runOneStep();
	//node5->runOneStep();
	//node7->runOneStep();
	//network->m_links.runOneStep();
	//network->m_routers.at(1).runOneStep();
	//network->m_routers.at(3).runOneStep();
	//network->m_routers.at(4).runOneStep();
	//network->m_routers.at(5).runOneStep();
	//network->m_routers.at(7).runOneStep();
	//globalClock.tickGlobalClock();

	//network->updateEnable();
	//node1->updateEnable();
	//node3->updateEnable();
	//node4->updateEnable();
	//node5->updateEnable();
	//node7->updateEnable();
	//node1->runOneStep();
	//node3->runOneStep();
	//node4->runOneStep();
	//node5->runOneStep();
	//node7->runOneStep();
	//network->m_links.runOneStep();
	//network->m_routers.at(1).runOneStep();
	//network->m_routers.at(3).runOneStep();
	//network->m_routers.at(4).runOneStep();
	//network->m_routers.at(5).runOneStep();
	//network->m_routers.at(7).runOneStep();
	//globalClock.tickGlobalClock();

	//network->updateEnable();
	//node1->updateEnable();
	//node3->updateEnable();
	//node4->updateEnable();
	//node5->updateEnable();
	//node7->updateEnable();
	//node1->runOneStep();
	//node3->runOneStep();
	//node4->runOneStep();
	//node5->runOneStep();
	//node7->runOneStep();
	//network->m_links.runOneStep();
	//network->m_routers.at(1).runOneStep();
	//network->m_routers.at(3).runOneStep();
	//network->m_routers.at(4).runOneStep();
	//network->m_routers.at(5).runOneStep();
	//network->m_routers.at(7).runOneStep();
	//globalClock.tickGlobalClock();

	//network->updateEnable();
	//node1->updateEnable();
	//node3->updateEnable();
	//node4->updateEnable();
	//node5->updateEnable();
	//node7->updateEnable();
	//node1->runOneStep();
	//node3->runOneStep();
	//node4->runOneStep();
	//node5->runOneStep();
	//node7->runOneStep();
	//network->m_links.runOneStep();
	//network->m_routers.at(1).runOneStep();
	//network->m_routers.at(3).runOneStep();
	//network->m_routers.at(4).runOneStep();
	//network->m_routers.at(5).runOneStep();
	//network->m_routers.at(7).runOneStep();
	//globalClock.tickGlobalClock();

	std::cout << Clock::s_globalClock << std::endl;
}