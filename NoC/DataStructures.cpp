#include "DataStructures.h"

std::ostream& operator<<(std::ostream& stream, const PortType& portType)
{
	switch (portType)
	{
	case PortType::Unselected:
		stream << "U";
		break;
	case PortType::NorthPort:
		stream << "N";
		break;
	case PortType::SouthPort:
		stream << "S";
		break;
	case PortType::WestPort:
		stream << "W";
		break;
	case PortType::EastPort:
		stream << "E";
		break;
	case PortType::TerminalPort:
		stream << "T";
		break;
	}
	return stream;
}

bool VirtualChannelArbitrationRecorderLine::operator==(const VirtualChannelArbitrationRecorderLine& record) const
{
	return port == record.port
		&& virtualChannel == record.virtualChannel;
}

bool SwitchArbitrationRecorderLine::operator==(const SwitchArbitrationRecorderLine& record) const
{
	return port == record.port
		&& outputPortSwitched == record.outputPortSwitched
		&& switchPriorityGlobal == record.switchPriorityGlobal;
}

std::ostream& operator<<(std::ostream& stream, const VirtualChannelState& virtualChannelState)
{
	switch (virtualChannelState)
	{
	case VirtualChannelState::I:
		stream << "I";
		break;
	case VirtualChannelState::R:
		stream << "R";
		break;
	case VirtualChannelState::V:
		stream << "V";
		break;
	case VirtualChannelState::A:
		stream << "A";
		break;
	}
	return stream;
}

std::ostream& operator<<(std::ostream& stream, const PacketType& packetType)
{
	switch (packetType)
	{
	case PacketType::Default:
		stream << "D";
		break;
	case PacketType::ReadRequest:
		stream << "RQ";
		break;
	case PacketType::ReadResponse:
		stream << "RB";
		break;
	case PacketType::WriteRequest:
		stream << "WQ";
		break;
	case PacketType::WriteResponse:
		stream << "WB";
		break;
	}
	return stream;
}

std::ostream& operator<<(std::ostream& stream, const Packet& packet)
{
	stream << " "
		<< packet.destination << "|"
		<< packet.xID << "|"
		<< packet.RWQB << "|"
		<< packet.MID << "|"
		<< packet.SID << "|"
		<< packet.AxADDR << "|"
		<< packet.xDATA
		<< " ";
	return stream;
}

std::ostream& operator<<(std::ostream& stream, const FlitType& flitType)
{
	switch (flitType)
	{
	case FlitType::DefaultFlit:
		stream << "D";
		break;
	case FlitType::HeadFlit:
		stream << "H";
		break;
	case FlitType::BodyFlit:
		stream << "B";
		break;
	case FlitType::TailFlit:
		stream << "T";
		break;
	case FlitType::HeadTailFlit:
		stream << "HT";
		break;
	}
	return stream;
}

std::ostream& operator<<(std::ostream& stream, const Flit& flit)
{
	stream << " "
		<< flit.port << "|"
		<< flit.virtualChannel << "|"
		<< flit.flitType << "|"
		<< flit.destination << "|"
		<< flit.xID << "|"
		<< flit.RWQB << "|"
		<< flit.MID << "|"
		<< flit.SID << "|"
		<< flit.AxADDR << "|"
		<< flit.xDATA
		<< " ";
	return stream;
}

std::ostream& operator<<(std::ostream& stream, const PEType& PEType)
{
	switch (PEType)
	{
	case PEType::HIDDEN:
		stream << "HIDDEN";
		break;
	case PEType::INPUT:
		stream << "INPUT";
		break;
	case PEType::OUTPUT:
		stream << "OUTPUT";
		break;
	}
	return stream;
}

std::ostream& operator<<(std::ostream& stream, const RouterID& routerID)
{
	stream << " (" << routerID.y << ", " << routerID.x << ") ";
	return stream;
}