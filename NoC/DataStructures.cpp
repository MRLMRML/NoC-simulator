#include "DataStructures.h"

std::ostream& operator<<(std::ostream& stream, const PortType& portType)
{
	switch (portType)
	{
	case PortType::Unselected:
		stream << "Unselected";
		break;
	case PortType::NorthPort:
		stream << "NorthPort";
		break;
	case PortType::SouthPort:
		stream << "SouthPort";
		break;
	case PortType::WestPort:
		stream << "WestPort";
		break;
	case PortType::EastPort:
		stream << "EastPort";
		break;
	case PortType::TerminalPort:
		stream << "TerminalPort";
		break;
	}
	return stream;
}

std::ostream& operator<<(std::ostream& stream, const VirtualChannelState& virtualChannelState)
{
	switch (virtualChannelState)
	{
	case VirtualChannelState::I:
		stream << "Idle";
		break;
	case VirtualChannelState::R:
		stream << "Route computation";
		break;
	case VirtualChannelState::V:
		stream << "Virtual channel allocation";
		break;
	case VirtualChannelState::A:
		stream << "Active";
		break;
	case VirtualChannelState::T:
		stream << "Terminate";
		break;
	}
	return stream;
}

std::ostream& operator<<(std::ostream& stream, const PacketType& packetType)
{
	switch (packetType)
	{
	case PacketType::ReadRequest:
		stream << "ReadRequest";
		break;
	case PacketType::ReadResponse:
		stream << "ReadResponse";
		break;
	case PacketType::WriteRequest:
		stream << "WriteRequest";
		break;
	case PacketType::WriteResponse:
		stream << "WriteResponse";
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
	case FlitType::HeadFlit:
		stream << "HeadFlit";
		break;
	case FlitType::BodyFlit:
		stream << "BodyFlit";
		break;
	case FlitType::TailFlit:
		stream << "TailFlit";
		break;
	case FlitType::HeadTailFlit:
		stream << "HeadTailFlit";
		break;
	}
	return stream;
}

std::ostream& operator<<(std::ostream& stream, const Flit& flit)
{
	stream << " "
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