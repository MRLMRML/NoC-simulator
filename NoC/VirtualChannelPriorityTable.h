#pragma once
#include "DataStructures.h"

class VirtualChannelPriorityTable
{
public:
	VirtualChannelPriorityTable() = default;

private:
	std::array<VirtualChannel&, 4 * BUFFER_SIZE> m_table{};

};