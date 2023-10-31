#include "DopplerNode.h"

void DopplerNode::runOneStep()
{
	if (!m_isSilent)
		injectTraffic();
	collectTraffic();
	m_localClock.synchronizeTriggerClock();
	m_localClock.synchronizeExecutionClock();
}

void DopplerNode::viewPacket(const Packet& packet)
{
#if defined (DEBUG)
	std::cout << packet << std::endl;
#endif
}

void DopplerNode::viewFlit(const Flit& flit)
{
#if defined (DEBUG)
	std::cout << flit << std::endl;
#endif
}

void DopplerNode::injectTraffic()
{
	if (m_localClock.triggerLocalEvent())
	{
		if (!m_localClock.isWaitingForExecution())
		{
			m_localClock.tickExecutionClock(EXECUTION_TIME_DOPPLERNODE_INJECTTRAFFIC - 1);
			m_localClock.toggleWaitingForExecution();
		}

		if (m_localClock.executeLocalEvent())
		{
			if (readPacket())
			{
				viewPacket(m_packetGenerated);
				dismantlePacket();
				recordInputTime(m_localClock.s_globalClock + 1);

				m_localClock.tickTriggerClock(PERIOD_DOPPLERNODE_INJECTTRAFFIC - EXECUTION_TIME_DOPPLERNODE_INJECTTRAFFIC + 1);
				m_localClock.tickExecutionClock(PERIOD_DOPPLERNODE_INJECTTRAFFIC - EXECUTION_TIME_DOPPLERNODE_INJECTTRAFFIC + 1);
				m_localClock.toggleWaitingForExecution();
			}
		}
	}

	sendFlit(); // every cycle; it may stall when network is busy
}

bool DopplerNode::readPacket()
{
	// read packet
	// open the file
	std::ifstream readPacket(g_dataFolderPath + g_packetRecordPath, std::ios::in);
	// preparation 
	std::string line{};
	std::string source{};
	std::string packetID{};
	std::string destination{};
	std::string status{};
	std::string inputTime{};
	std::string outputTime{};
	std::istringstream lineInString{};
	// read the head line
	std::getline(readPacket, line);
	// read file line by line
	while (std::getline(readPacket, line))
	{
		lineInString.str(line);
		// read each data field
		std::getline(lineInString, source, ',');
		std::getline(lineInString, packetID, ',');
		std::getline(lineInString, destination, ',');
		std::getline(lineInString, status, ',');
		std::getline(lineInString, inputTime, ',');
		std::getline(lineInString, outputTime, ',');
		// find the correct packet to sent
		if (source == std::to_string(m_NID) 
			&& packetID == std::to_string(m_packetIDTracker)
			&& status == "intact")
		{
			m_packetGenerated.destination = std::stoi(destination);
			m_packetGenerated.xID = std::stoi(packetID);
			m_packetGenerated.RWQB = PacketType::Default;
			m_packetGenerated.MID = std::stoi(source);
			m_packetGenerated.SID = std::stoi(destination);

			m_packetIDTracker++;
			readPacket.close();
			return true;
		}
	}
	readPacket.close();
	return false;
}

void DopplerNode::dismantlePacket()
{
	double flitCount{ ceil(PACKET_SIZE / FLIT_SIZE) }; // number of flits in total
	//double flitCount{ ceil(sizeof(m_packetGenerated) / FLIT_SIZE) }; // number of flits in total

	if (flitCount == 1) // H/T flit
	{
		Flit headTailFlit{ PortType::Unselected, -1, FlitType::HeadTailFlit, m_packetGenerated };
		m_sourceQueue.push_back(headTailFlit);
		viewFlit(headTailFlit);
		return;
	}

	if (flitCount == 2) // no BodyFlit
	{
		Flit headFlit{ PortType::Unselected, -1, FlitType::HeadFlit, m_packetGenerated.destination, m_packetGenerated.xID,
			m_packetGenerated.RWQB, m_packetGenerated.MID, m_packetGenerated.SID, m_packetGenerated.SEQID };
		m_sourceQueue.push_back(headFlit);
		viewFlit(headFlit);

		Flit tailFlit{ PortType::Unselected, -1, FlitType::TailFlit, m_packetGenerated.xID, m_packetGenerated.MID, m_packetGenerated.SEQID,
			m_packetGenerated.AxADDR, m_packetGenerated.xDATA };
		m_sourceQueue.push_back(tailFlit);
		viewFlit(tailFlit);
		return;
	}

	// HeadFlit, BodyFlit x (flitCount - 2), TailFlit
	// HeadFlit
	Flit headFlit{ PortType::Unselected, -1, FlitType::HeadFlit, m_packetGenerated.destination, m_packetGenerated.xID,
			m_packetGenerated.RWQB, m_packetGenerated.MID, m_packetGenerated.SID, m_packetGenerated.SEQID };
	m_sourceQueue.push_back(headFlit);
	viewFlit(headFlit);

	// BodyFlit
	for (int i{}; i < (flitCount - 2); ++i)
	{
		Flit bodyFlit{ PortType::Unselected, -1, FlitType::BodyFlit, i+1, m_packetGenerated.xID, m_packetGenerated.MID, m_packetGenerated.SEQID };
		m_sourceQueue.push_back(bodyFlit);
		viewFlit(bodyFlit);
	}

	// TailFlit
	Flit tailFlit{ PortType::Unselected, -1, FlitType::TailFlit, m_packetGenerated.xID, m_packetGenerated.MID, m_packetGenerated.SEQID,
			m_packetGenerated.AxADDR, m_packetGenerated.xDATA };
	m_sourceQueue.push_back(tailFlit);
	viewFlit(tailFlit);
}

void DopplerNode::recordInputTime(const float packetInputTime)
{
	// record input time
	// open the current file
	std::ifstream readPacketRecord(g_dataFolderPath + g_packetRecordPath, std::ios::in);
	// create a new file
	std::string t_packetRecord{ g_dataFolderPath + "t_PacketRecord.csv" };
	std::ofstream t_readPacketRecord(t_packetRecord);
	// preparation 
	std::string line{};
	std::string source{};
	std::string packetID{};
	std::string destination{};
	std::string status{};
	std::string inputTime{};
	std::string outputTime{};
	std::istringstream lineInString{};
	// read the head line
	std::getline(readPacketRecord, line);
	// write the head line into the new file
	t_readPacketRecord << line << std::endl;
	// read current file line by line
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
		// decide whether to change each line, and/or write it into a new file
		if (source == std::to_string(m_packetGenerated.MID) && packetID == std::to_string(m_packetGenerated.xID))
		{
			t_readPacketRecord
				<< source << ","
				<< packetID << ","
				<< destination << ","
				<< "sent" << ","
				<< std::to_string(packetInputTime) << ","
				<< outputTime << ","
				<< std::endl;
		}
		else
		{
			t_readPacketRecord << line << std::endl;
		}
	}
	readPacketRecord.close();
	t_readPacketRecord.close();
	// when finished, delete the current file and rename the new file to the deleted file
	std::remove((g_dataFolderPath + g_packetRecordPath).c_str());
	std::rename(t_packetRecord.c_str(), (g_dataFolderPath + g_packetRecordPath).c_str());
}

void DopplerNode::sendFlit()
{
	if (!m_sourceQueue.empty()
		&& m_port.m_outFlitRegister.size() < REGISTER_DEPTH)
	{
		m_port.m_outFlitRegister.push_back(m_sourceQueue.front());
		m_sourceQueue.pop_front();
	}
}

void DopplerNode::collectTraffic()
{
	assemblePacket();
}

bool DopplerNode::receiveFlit()
{
	if (!m_port.m_inFlitRegister.empty())
	{
		Flit flit{ m_port.m_inFlitRegister.front()};
		m_port.m_inFlitRegister.pop_front();
		m_flitReorderBuffer.push_back(flit);
		return true;
	}
	else
		return false;
}

void DopplerNode::assemblePacket()
{
	if (receiveFlit())
	{
		if (m_flitReorderBuffer.back().flitType == FlitType::HeadTailFlit)
		{
			m_packetReceived.destination = m_flitReorderBuffer.back().destination;
			m_packetReceived.xID = m_flitReorderBuffer.back().xID;
			m_packetReceived.RWQB = m_flitReorderBuffer.back().RWQB;
			m_packetReceived.MID = m_flitReorderBuffer.back().MID;
			m_packetReceived.SID = m_flitReorderBuffer.back().SID;
			m_packetReceived.SEQID = m_flitReorderBuffer.back().SEQID;
			m_packetReceived.AxADDR = m_flitReorderBuffer.back().AxADDR;
			m_packetReceived.xDATA = m_flitReorderBuffer.back().xDATA;
			viewPacket(m_packetReceived);
			m_flitReorderBuffer.pop_back();
			recordOutputTime(m_localClock.s_globalClock + 1);
			return;
		}

		if (m_flitReorderBuffer.back().flitType == FlitType::TailFlit)
		{
			std::vector<Flit> t_flitReorderBuffer{ m_flitReorderBuffer };
			for (auto& flit : t_flitReorderBuffer) // find the flits that have the same IDs and erase them from flit reorder buffer
			{
				if (flit.xID == m_flitReorderBuffer.back().xID &&
					flit.MID == m_flitReorderBuffer.back().MID &&
					flit.SEQID == m_flitReorderBuffer.back().SEQID)
				{
					if (flit.flitType == FlitType::HeadFlit) // find the head flit and get the data
					{
						m_packetReceived.destination = flit.destination;
						m_packetReceived.xID = flit.xID;
						m_packetReceived.RWQB = flit.RWQB;
						m_packetReceived.MID = flit.MID;
						m_packetReceived.SID = flit.SID;
						m_packetReceived.SEQID = flit.SEQID;
						m_packetReceived.AxADDR = m_flitReorderBuffer.back().AxADDR;
						m_packetReceived.xDATA = m_flitReorderBuffer.back().xDATA;
						viewPacket(m_packetReceived);
						recordOutputTime(m_localClock.s_globalClock + 1);
					}
					std::erase(m_flitReorderBuffer, flit); // C++20
					//m_flitReorderBuffer.erase(std::remove(m_flitReorderBuffer.begin(), m_flitReorderBuffer.end(), flit), m_flitReorderBuffer.end());
				}
			}
			return;
		}
	}
}

void DopplerNode::recordOutputTime(const float packetOutputTime)
{
	// record output time
	// open the current file
	std::ifstream readPacketRecord(g_dataFolderPath + g_packetRecordPath, std::ios::in);
	// create a new file
	std::string t_packetRecord{ g_dataFolderPath + "t_PacketRecord.csv" };
	std::ofstream t_readPacketRecord(t_packetRecord);
	// preparation 
	std::string line{};
	std::string source{};
	std::string packetID{};
	std::string destination{};
	std::string status{};
	std::string inputTime{};
	std::string outputTime{};
	std::istringstream lineInString{};
	// read the head line
	std::getline(readPacketRecord, line);
	// write the head line into the new file
	t_readPacketRecord << line << std::endl;
	// read current file line by line
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
		// decide whether to change each line, and/or write it into a new file
		if (source == std::to_string(m_packetReceived.MID) && packetID == std::to_string(m_packetReceived.xID))
		{
			t_readPacketRecord
				<< source << ","
				<< packetID << ","
				<< destination << ","
				<< "received" << ","
				<< inputTime << ","
				<< std::to_string(packetOutputTime) << ","
				<< std::endl;
		}
		else
		{
			t_readPacketRecord << line << std::endl;
		}
	}
	readPacketRecord.close();
	t_readPacketRecord.close();
	// when finished, delete the current file and rename the new file to the deleted file
	std::remove((g_dataFolderPath + g_packetRecordPath).c_str());
	std::rename(t_packetRecord.c_str(), (g_dataFolderPath + g_packetRecordPath).c_str());
}