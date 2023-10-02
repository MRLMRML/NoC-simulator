#include "Links.h"

void Links::setUpConnection(Port& inputPort, Port& outputPort)
{
	m_connections.insert({ &inputPort, &outputPort });
	log(" Links: connection set up ");
}

void Links::terminateConnection(Port& inputPort, Port& outputPort)
{
	m_connections.erase({ &inputPort, &outputPort });
	log(" Links: connection terminated ");
}

void Links::terminateAllConnections()
{
	m_connections.clear();
	log(" Links: all connections terminated ");
}

//bool Links::areEmpty()
//{
//	bool areEmpty{ true };
//	for (auto& connection : m_connections)
//	{
//		areEmpty = areEmpty && connection.first->isEmpty() && connection.second->isEmpty();
//	}
//	return areEmpty;
//}

void Links::runOneStep()
{
	// transfer one element in Port pair bidirectionaliy
	for (auto& connection : m_connections)
	{
		if (connection.first->m_outFlitRegister.valid == true 
			&& connection.second->m_inFlitRegister.valid == false)
		{
			connection.second->m_inFlitRegister.flit = connection.first->m_outFlitRegister.flit;
			connection.first->m_outFlitRegister.valid = false;
			connection.second->m_inFlitRegister.valid = true;
			log(" Links: flit transferred (LHS -> RHS) ");
		}

		if (connection.second->m_outCreditRegister.valid == true
			&& connection.first->m_inCreditRegister.valid == false)
		{
			connection.first->m_inCreditRegister.credit = connection.second->m_outCreditRegister.credit;
			connection.second->m_outCreditRegister.valid = false;
			connection.first->m_inCreditRegister.valid = true;
			log(" Links: credit transferred (LHS <- RHS) ");
		}

		if (connection.second->m_outFlitRegister.valid == true
			&& connection.first->m_inFlitRegister.valid == false)
		{
			connection.first->m_inFlitRegister.flit = connection.second->m_outFlitRegister.flit;
			connection.second->m_outFlitRegister.valid = false;
			connection.first->m_inFlitRegister.valid = true;
			log(" Links: flit transferred (LHS <- RHS) ");
		}

		if (connection.first->m_outCreditRegister.valid == true
			&& connection.second->m_inCreditRegister.valid == false)
		{
			connection.second->m_inCreditRegister.credit = connection.first->m_outCreditRegister.credit;
			connection.first->m_outCreditRegister.valid = false;
			connection.second->m_inCreditRegister.valid = true;
			log(" Links: credit transferred (LHS <- RHS) ");
		}
	}
}