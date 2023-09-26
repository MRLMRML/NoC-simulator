#include "Network.h"

int main()
{
	// create doppler nodes
	DopplerNode* node0{ new DopplerNode{} };

	// create network
	Network* network{ new Network{} };
	network->setUpConnections();
	network->printRouterIDs();

	// mount nodes on the network
	network->mountNode({0, 0}, node0);

	network->printNodeIDs();
	network->viewMappingTable();
	network->updateMappingTables();

	// testing
	node0->runOneStep();
	while(true)
		network->runOneStep();
}