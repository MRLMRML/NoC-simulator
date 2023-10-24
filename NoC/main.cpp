#include <random>
#include "Network.h"

// global variables
std::string g_dataFolderPath{ "C:\\Users\\Hubiao\\source\\repos\\NoC\\NoC\\Data\\" };
std::string g_packetRecordPath{ "PacketRecord.csv" };

void randomUniformTraffic()
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
	
	std::random_device rd;
	std::mt19937 gen(rd());  // to seed mersenne twister.
	std::uniform_int_distribution<> dist(0, NETWORK_DIMENSION_X * NETWORK_DIMENSION_Y);

	int destination{};
	for (int i{}; i < PACKET_NUMBER; ++i)
	{
		for (int j{}; j < NETWORK_DIMENSION_X * NETWORK_DIMENSION_Y; ++j)
		{
			do
			{
				destination = dist(gen);
			} while (destination == j);

			writePacketRecord
				<< j << ","
				<< i << ","
				<< destination << ","
				<< "intact" << ","
				<< "-" << ","
				<< "-" << ","
				<< std::endl;
		}
	}

	writePacketRecord.close();
}

void allToOneTraffic()
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

	int destination{ DRAIN_NODE };
	for (int i{}; i < PACKET_NUMBER; ++i)
	{
		for (int j{}; j < NETWORK_DIMENSION_X * NETWORK_DIMENSION_Y; ++j)
		{
			if (j != destination)
			{
				writePacketRecord
					<< j << ","
					<< i << ","
					<< destination << ","
					<< "intact" << ","
					<< "-" << ","
					<< "-" << ","
					<< std::endl;
			}
		}
	}

	writePacketRecord.close();
}

void generateTraffic()
{
#if defined(RANDOM_UNIFORM)
	randomUniformTraffic();
#endif
	
#if defined(ALL_TO_ONE)
	allToOneTraffic();
#endif

#if defined(CUSTOMIZE)
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
		<< "0" << ","
		<< "0" << ","
		<< "0" << ","
		<< "intact" << ","
		<< "-" << ","
		<< "-" << ","
		<< std::endl;
	writePacketRecord.close();
#endif
}

int main()
{
	// generate traffic
	generateTraffic();

	// timing
	Clock globalClock{};

	// create network
	Network* network{ new Network{} };
	network->setUpConnections();
	network->printRouterIDs();

	// create doppler nodes
	std::vector<DopplerNode*> dopplerNodes;
	for (int i{}; i < NETWORK_DIMENSION_X * NETWORK_DIMENSION_Y; ++i)
	{
		DopplerNode* node{ new DopplerNode{i, false} };
		dopplerNodes.push_back(node);
	}

	// mount nodes on the network
	for (int i{}; i < NETWORK_DIMENSION_X * NETWORK_DIMENSION_Y; ++i)
	{
		network->mountNode({ i/NETWORK_DIMENSION_X, i% NETWORK_DIMENSION_Y }, dopplerNodes.at(i));
	}
	network->printNodeIDs();
	network->viewMappingTable();
	network->updateMappingTables();

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

	std::cout << Clock::s_globalClock << std::endl;

	// sanitation
	for (int i{}; i < NETWORK_DIMENSION_X * NETWORK_DIMENSION_Y; ++i)
		delete dopplerNodes.at(i);
	delete network;
}