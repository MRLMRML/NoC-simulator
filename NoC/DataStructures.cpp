#include "DataStructures.h"

std::ostream& operator<<(std::ostream& stream, const Direction& direction)
{
	switch (direction)
	{
	case Direction::N:
		stream << "N";
		break;
	case Direction::S:
		stream << "S";
		break;
	case Direction::W:
		stream << "W";
		break;
	case Direction::E:
		stream << "E";
		break;
	case Direction::X:
		stream << "X";
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