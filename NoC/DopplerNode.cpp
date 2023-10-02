#include "DopplerNode.h"

void DopplerNode::runOneStep()
{
	injectTraffic();
	collectTraffic();
}

void DopplerNode::viewPacket(const Packet& packet)
{
	std::cout << packet << std::endl;
}

void DopplerNode::viewFlit(const Flit& flit)
{
	std::cout << flit << std::endl;
}

void DopplerNode::injectTraffic()
{
	// packet rate??
	// if generate packet:
		generatePacket();
		viewPacket(m_packetGenerated);
		dismantlePacket();
		recordInputTime();

	sendFlit(); // every cycle; it may stall when network is busy
}

void DopplerNode::generatePacket()
{
	m_packetGenerated.xDATA = { 1, 1, 4, 5, 1, 4 };
}

void DopplerNode::dismantlePacket()
{
	double flitCount{ ceil(sizeof(m_packetGenerated) / FLIT_SIZE) }; // number of flits in total

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
	for (size_t i{}; i < (flitCount - 2); ++i)
	{
		Flit bodyFlit{ PortType::Unselected, -1, FlitType::BodyFlit, m_packetGenerated.xID, m_packetGenerated.MID, m_packetGenerated.SEQID };
		m_sourceQueue.push_back(bodyFlit);
		viewFlit(bodyFlit);
	}

	// TailFlit
	Flit tailFlit{ PortType::Unselected, -1, FlitType::TailFlit, m_packetGenerated.xID, m_packetGenerated.MID, m_packetGenerated.SEQID,
			m_packetGenerated.AxADDR, m_packetGenerated.xDATA };
	m_sourceQueue.push_back(tailFlit);
	viewFlit(tailFlit);
}

void DopplerNode::recordInputTime()
{
	// record input time
}

void DopplerNode::sendFlit()
{
	if (m_port.m_outFlitRegister.valid == false)
	{
		m_port.m_outFlitRegister.flit = m_sourceQueue.front();
		m_sourceQueue.pop_front();
		m_port.m_outFlitRegister.valid = true;
	}
}

void DopplerNode::collectTraffic()
{
	assemblePacket();
}

bool DopplerNode::receiveFlit()
{
	if (m_port.m_inFlitRegister.valid == true)
	{
		Flit flit{ m_port.m_inFlitRegister.flit };
		m_port.m_inFlitRegister.valid = false;
		m_flitReorderBuffer.push_back(flit);
		return true;
	}
	else
		return false;
}

void DopplerNode::assemblePacket()
{
	if (receiveFlit() == true)
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
			recordOutputTime();
			return;
		}

		if (m_flitReorderBuffer.back().flitType == FlitType::TailFlit)
		{
			for (auto& headFlit : m_flitReorderBuffer) // find the headflit that has the same IDs
			{
				if ((headFlit.flitType == FlitType::HeadFlit) &&
					(headFlit.xID == m_flitReorderBuffer.back().xID) &&
					(headFlit.MID == m_flitReorderBuffer.back().MID) &&
					(headFlit.SEQID == m_flitReorderBuffer.back().SEQID))
				{
					m_packetReceived.destination = headFlit.destination;
					m_packetReceived.xID = headFlit.xID;
					m_packetReceived.RWQB = headFlit.RWQB;
					m_packetReceived.MID = headFlit.MID;
					m_packetReceived.SID = headFlit.SID;
					m_packetReceived.SEQID = headFlit.SEQID;
					m_packetReceived.AxADDR = m_flitReorderBuffer.back().AxADDR;
					m_packetReceived.xDATA = m_flitReorderBuffer.back().xDATA;
					recordOutputTime();
					return;
				}
			}
		}
	}
}

void DopplerNode::recordOutputTime()
{
	// record output time
}