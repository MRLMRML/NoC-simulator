#include "DopplerNode.h"

void DopplerNode::runOneStep()
{
	generatePacket();
	assemblePacket();
}

void DopplerNode::generatePacket()
{
	Packet packet{
	0,
	0,
	PacketType::WriteRequest,
	0,
	1,
	0,
	0,
		{1, 1, 4, 5, 1, 4} }; // random? file I/O?
	viewPacket(packet);
	sendPacket(packet);
}

void DopplerNode::viewPacket(const Packet& packet)
{
	std::cout << packet << std::endl;
}

void DopplerNode::viewFlit(const Flit& flit)
{
	std::cout << flit << std::endl;
}

void DopplerNode::sendPacket(const Packet packet)
{
	dismantlePacket(packet);
	log(" DopplerNode: packet sent ");
}

void DopplerNode::dismantlePacket(const Packet packet)
{
	double flitCount{ ceil(sizeof(packet) / FLIT_SIZE) }; // number of flits in total

	if (flitCount == 1) // H/T flit
	{
		Flit headTailFlit{ PortType::Unselected, -1, FlitType::HeadTailFlit, packet };
		m_port.m_outFlitBuffer.push_back(headTailFlit);
		viewFlit(headTailFlit);
		return;
	}

	if (flitCount == 2) // no BodyFlit
	{
		Flit headFlit{ PortType::Unselected, -1, FlitType::HeadFlit, packet.destination, packet.xID,
			packet.RWQB, packet.MID, packet.SID, packet.SEQID };
		m_port.m_outFlitBuffer.push_back(headFlit);
		viewFlit(headFlit);

		Flit tailFlit{ PortType::Unselected, -1, FlitType::TailFlit, packet.xID, packet.MID, packet.SEQID,
			packet.AxADDR, packet.xDATA };
		m_port.m_outFlitBuffer.push_back(tailFlit);
		viewFlit(tailFlit);
		return;
	}

	// HeadFlit, BodyFlit x (flitCount - 2), TailFlit
	// HeadFlit
	Flit headFlit{ PortType::Unselected, -1, FlitType::HeadFlit, packet.destination, packet.xID,
			packet.RWQB, packet.MID, packet.SID, packet.SEQID };
	m_port.m_outFlitBuffer.push_back(headFlit);
	viewFlit(headFlit);

	// BodyFlit
	for (size_t i{}; i < (flitCount - 2); ++i)
	{
		Flit bodyFlit{ PortType::Unselected, -1, FlitType::BodyFlit, packet.xID, packet.MID, packet.SEQID };
		m_port.m_outFlitBuffer.push_back(bodyFlit);
		viewFlit(bodyFlit);
	}

	// TailFlit
	Flit tailFlit{ PortType::Unselected, -1, FlitType::TailFlit, packet.xID, packet.MID, packet.SEQID,
			packet.AxADDR, packet.xDATA };
	m_port.m_outFlitBuffer.push_back(tailFlit);
	viewFlit(tailFlit);
}

void DopplerNode::assemblePacket()
{
	if (!m_port.m_inFlitBuffer.empty())
	{
		Flit flit{ m_port.m_inFlitBuffer.front() };
		m_port.m_inFlitBuffer.pop_front();

		if (flit.flitType == FlitType::HeadTailFlit)
		{
			Packet packet{ flit.destination, flit.xID, flit.RWQB, flit.MID,
			flit.SID, flit.SEQID, flit.AxADDR, flit.xDATA };
			receivePacket(packet);
			return;
		}

		if (flit.flitType == FlitType::TailFlit)
		{
			for (auto& headFlit : m_flitReorderBuffer) // find the headflit that has the same IDs
			{
				if ((headFlit.flitType == FlitType::HeadFlit) &&
					(headFlit.xID == flit.xID) &&
					(headFlit.MID == flit.MID) &&
					(headFlit.SEQID == flit.SEQID))
				{
					Packet packet{ headFlit.destination, headFlit.xID,
						headFlit.RWQB, headFlit.MID, headFlit.SID,
						headFlit.SEQID, flit.AxADDR, flit.xDATA };
					receivePacket(packet);
					return;
				}
			}
		}

		m_flitReorderBuffer.push_back(flit);
	}
}

void DopplerNode::receivePacket(const Packet packet)
{
	log(" DopplerNode: packet received ");
	viewPacket(packet);
}