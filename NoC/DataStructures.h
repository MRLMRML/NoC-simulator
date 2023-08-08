#pragma once
#include "Parameters.h"
#include <vector>
#include <deque>
#include <iostream>

// << overloading for std::vector
template <typename T> std::ostream& operator<<(std::ostream& stream, const std::vector<T>& vector)
{
	for (auto element : vector)
	{
		stream << element << " ";
	}
	return stream;
}

// << overloading for std::deque
template <typename T> std::ostream& operator<<(std::ostream& stream, const std::deque<T>& deque)
{
	for (auto element : deque)
	{
		stream << element << " ";
	}
	return stream;
}

struct MemoryLine
{
	int address{};
	std::vector<DATA_PRECISION> data{};
};

enum class Direction
{
	N,
	S,
	W,
	E,
	X
};

std::ostream& operator<<(std::ostream& stream, const Direction& direction);

enum class PacketType
{
	ReadRequest,
	ReadResponse,
	WriteRequest,
	WriteResponse
};

std::ostream& operator<<(std::ostream& stream, const PacketType& packetType);

struct Packet
{
	int destination{};
	int xID{};
	PacketType RWQB{};
	int MID{};
	int SID{};
	int SEQID{};
	int AxADDR{};
	std::vector<DATA_PRECISION> xDATA{};
};

std::ostream& operator<<(std::ostream& stream, const Packet& packet);

struct PacketReorderBufferLine
{
	int MID{};
	std::vector<DATA_PRECISION> p_xData{};
};

enum class FlitType
{
	HeadFlit,
	BodyFlit,
	TailFlit,
	HeadTailFlit
};

std::ostream& operator<<(std::ostream& stream, const FlitType& flitType);

struct Flit
{
	// HeadFlit
	Flit(const FlitType flitType,
		const int destination,
		const int xID,
		const PacketType RWQB,
		const int MID,
		const int SID,
		const int SEQID)
		:
		flitType{ flitType },
		destination{ destination },
		xID{ xID },
		RWQB{ RWQB },
		MID{ MID },
		SID{ SID },
		SEQID{ SEQID } {}

	// BodyFlit
	Flit(const FlitType flitType,
		const int xID,
		const int MID,
		const int SEQID)
		:
		flitType{ flitType },
		xID{ xID },
		MID{ MID },
		SEQID{ SEQID } {}

	// TailFlit
	Flit(const FlitType flitType,
		const int xID,
		const int MID,
		const int SEQID,
		const int AxADDR,
		const std::vector<DATA_PRECISION> xDATA)
		:
		flitType{ flitType },
		xID{ xID },
		MID{ MID },
		SEQID{ SEQID },
		AxADDR{ AxADDR },
		xDATA{ xDATA } {}

	// HeadTailFlit
	Flit(const FlitType flitType,
		const Packet packet)
		:
		flitType{ flitType },
		destination{ packet.destination },
		xID{ packet.xID },
		RWQB{ packet.RWQB },
		MID{ packet.MID },
		SID{ packet.SID },
		SEQID{ packet.SEQID },
		AxADDR{ packet.AxADDR },
		xDATA{ packet.xDATA } {}

	FlitType flitType{};
	int destination{};
	int xID{};
	PacketType RWQB{};
	int MID{};
	int SID{};
	int SEQID{};
	int AxADDR{};
	std::vector<DATA_PRECISION> xDATA{};
};

std::ostream& operator<<(std::ostream& stream, const Flit& flit);

//struct FlitReorderBufferLine
//{
//	
//};

enum class PEType
{
	HIDDEN,
	INPUT,
	OUTPUT
};

std::ostream& operator<<(std::ostream& stream, const PEType& packetType);

struct RouterID
{
	int y{}, x{};
};

std::ostream& operator<<(std::ostream& stream, const RouterID& routerID);

struct MappingTableLine
{
	RouterID routerID{};
	int NID{};
};
