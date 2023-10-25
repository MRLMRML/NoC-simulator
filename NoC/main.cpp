#include <random>
#include "Network.h"

// global variables
std::string g_dataFolderPath{ "C:\\Users\\Hubiao\\source\\repos\\NoC\\NoC\\Data\\" };
std::string g_packetRecordPath{ "PacketRecord.csv" };
std::string g_trafficDataPath{ "TrafficData.csv" };

void uniformTraffic()
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
	//std::mt19937 gen(rd());  // to seed mersenne twister
	std::mt19937 gen(42);  // to reproduce random numbers
	std::uniform_int_distribution<> dist(0, ROUTER_NUMBER - 1);

	int destination{};
	for (int i{}; i < PACKET_NUMBER; ++i)
	{
		for (int j{}; j < ROUTER_NUMBER; ++j)
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

void permutationTraffic()
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
		for (int j{}; j < ROUTER_NUMBER; ++j)
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

void customizeTraffic()
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
		<< "0" << ","
		<< "0" << ","
		<< "0" << ","
		<< "intact" << ","
		<< "-" << ","
		<< "-" << ","
		<< std::endl;
	writePacketRecord.close();
}

void generateTraffic()
{
#if defined(UNIFORM)
	uniformTraffic();
#endif
	
#if defined(PERMUTATION)
	permutationTraffic();
#endif

#if defined(CUSTOMIZE)
	customizeTraffic();
#endif
}

void collectData()
{
	// read open PacketRecord.csv
	std::ifstream readPacketRecord(g_dataFolderPath + g_packetRecordPath, std::ios::in);
	// write open TrafficData.csv
	std::ofstream writeTrafficData(g_dataFolderPath + g_trafficDataPath, std::ios::out);
	writeTrafficData
		<< "Source" << ","
		<< "Packet_ID" << ","
		<< "Destination" << ","
		<< "Status" << ","
		<< "Latency" << ","
		<< std::endl;
	// preparation 
	std::string line{};
	std::string source{};
	std::string packetID{};
	std::string destination{};
	std::string status{};
	std::string inputTime{};
	std::string outputTime{};
	std::istringstream lineInString{};
	// read PacketRecord.csv head line
	std::getline(readPacketRecord, line);
	// read PacketRecord.csv line by line
	while (std::getline(readPacketRecord, line))
	{
		lineInString.str(line);
		// read each data field
		std::getline(lineInString, source, ',');
		std::getline(lineInString, packetID, ',');
		std::getline(lineInString, destination, ',');
		std::getline(lineInString, status, ',');
		std::getline(lineInString, inputTime, ',');
		std::getline(lineInString, outputTime, ',');
		// write it into TrafficData.csv if it is sent out in measurement phase
		if (status != "intact")
		{
			if (std::stoi(inputTime) >= WARMUP_CYCLES && std::stoi(inputTime) < (WARMUP_CYCLES + MEASUREMENT_CYCLES))
			{
				if (status == "received")
				{
					writeTrafficData
						<< source << ","
						<< packetID << ","
						<< destination << ","
						<< status << ","
						<< std::stoi(outputTime) - std::stoi(inputTime) - 1 << ","
						<< std::endl;
				}
				else
				{
					writeTrafficData
						<< source << ","
						<< packetID << ","
						<< destination << ","
						<< status << ","
						<< "-" << ","
						<< std::endl;
				}
			}
		}
	}

	readPacketRecord.close();
	writeTrafficData.close();
}

NetworkPerformance calculatePerformance()
{
	// read open TrafficData.csv
	std::ifstream readTrafficData(g_dataFolderPath + g_trafficDataPath, std::ios::in);
	// preparation 
	std::string line{};
	std::string source{};
	std::string packetID{};
	std::string destination{};
	std::string status{};
	std::string latency{};
	std::istringstream lineInString{};
	float receivedPacketNumber{};
	float accumulatedLatency{};
	// read TrafficData.csv head line
	std::getline(readTrafficData, line);
	while (std::getline(readTrafficData, line))
	{
		lineInString.str(line);
		// read each data field
		std::getline(lineInString, source, ',');
		std::getline(lineInString, packetID, ',');
		std::getline(lineInString, destination, ',');
		std::getline(lineInString, status, ',');
		std::getline(lineInString, latency, ',');
		// calculate average latency, throughput
		if (status == "received") accumulatedLatency += std::stof(latency);
		if (status == "received") receivedPacketNumber++;
	}
	readTrafficData.close();
	float averageLatency{ accumulatedLatency / receivedPacketNumber };
	float throughput{ receivedPacketNumber * FLIT_NUMBER_PER_PACKET / (static_cast<float>(MEASUREMENT_CYCLES) * ROUTER_NUMBER) };
	return { averageLatency, throughput };
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
	// random initial phase
	std::random_device rd;
	//std::mt19937 gen(rd());  // to seed mersenne twister
	std::mt19937 gen(42);  // to reproduce random numbers
	std::uniform_int_distribution<> dist(0, PERIOD_DOPPLERNODE_INJECTTRAFFIC - 1);
	for (int i{}; i < ROUTER_NUMBER; ++i)
	{
		DopplerNode* node{ new DopplerNode{i, false, static_cast<float>(dist(gen))}};
		dopplerNodes.push_back(node);
	}

	// mount nodes on the network
	for (int i{}; i < ROUTER_NUMBER; ++i)
	{
		network->mountNode({ i/NETWORK_DIMENSION_X, i% NETWORK_DIMENSION_Y }, dopplerNodes.at(i));
	}
	network->printNodeIDs();
	network->viewMappingTable();
	network->updateMappingTables();

	//for (int i{}; i < SIMULATION_CYCLES; ++i)
	//{
	//	// update enable signals
	//	network->updateEnable();
	//	for (auto& node : dopplerNodes)
	//	{
	//		node->updateEnable();
	//	}

	//	for (auto& node : dopplerNodes)
	//	{
	//		node->runOneStep();
	//	}
	//	network->runOneStep();
	//	globalClock.tickGlobalClock();
	//}

	log(" ------------------------------- ");
	log(" Network starts warming up...... ");
	for (int i{}; i < WARMUP_CYCLES; ++i)
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
	std::cout << " Network warmed up for " << WARMUP_CYCLES << " cycles " << std::endl;

	log(" ------------------------------- ");
	log(" Network starts measuring...... ");
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
	std::cout << " Network measured for " << MEASUREMENT_CYCLES << " cycles " << std::endl;

	log(" ------------------------------- ");
	log(" Network starts draining...... ");
	for (int i{}; i < DRAIN_CYCLES; ++i)
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
	std::cout << " Network drained for " << DRAIN_CYCLES << " cycles " << std::endl;

	log(" ------------------------------- ");
	std::cout << " Simulation stopped at cycle: " << Clock::s_globalClock << std::endl;

	// collect data
	collectData();

	// calculate performance
	// network capacity in flit per cycle
#if defined (MESH)
	float networkCapacity{ 4.0f * NETWORK_BANDWIDTH / NETWORK_DIMENSION_X };
#endif
#if defined (TORUS)
	float networkCapacity{ 8.0f * NETWORK_BANDWIDTH / NETWORK_DIMENSION_X };
#endif

	// injection rate in flit per cycle
#if defined (PACKET_PER_CYCLE)
	float injectionRate{ INJECTION_RATE * FLIT_NUMBER_PER_PACKET };
#endif
#if defined (FLIT_PER_CYCLE)
	float injectionRate{ INJECTION_RATE };
#endif

	// offered traffic in fraction of capacity
	float offeredTraffic{ injectionRate / networkCapacity };

	// network performance
	NetworkPerformance perf{ calculatePerformance() };
	std::cout << " Average latency (cycle): " << perf.averageLatency << std::endl;
	std::cout << " Injection rate (flit per cycle): " << injectionRate << std::endl;
	std::cout << " Throughput (flit per cycle): " << perf.throughput << std::endl;
	std::cout << " Offered traffic (fraction of capacity): " << offeredTraffic << std::endl;
	std::cout << " Throughput (fraction of capacity): " << perf.throughput / networkCapacity << std::endl;

	// sanitation
	for (int i{}; i < ROUTER_NUMBER; ++i)
		delete dopplerNodes.at(i);
	delete network;
}