#include "VirtualChannelPriorityTable.h"

//void VirtualChannelPriorityTable::viewTable()
//{
//	std::cout << " Terminal port: " << m_table.at(0) << std::endl;
//
//	for (int i{}; i < BUFFER_SIZE; ++i)
//	{
//		std::cout << " North port: " << m_table.at(i + 1);
//	}
//	std::cout << std::endl;
//
//	for (int i{}; i < BUFFER_SIZE; ++i)
//	{
//		std::cout << " South port: " << m_table.at(i + BUFFER_SIZE * 1 + 1);
//	}
//	std::cout << std::endl;
//
//	for (int i{}; i < BUFFER_SIZE; ++i)
//	{
//		std::cout << " West port: " << m_table.at(i + BUFFER_SIZE * 2 + 1);
//	}
//	std::cout << std::endl;
//
//	for (int i{}; i < BUFFER_SIZE; ++i)
//	{
//		std::cout << " East port: " << m_table.at(i + BUFFER_SIZE * 3 + 1);
//	}
//	std::cout << std::endl;
//}
//
//void VirtualChannelPriorityTable::updateTable(const int index)
//{
//	for (auto& element : m_table)
//		if (element > m_table.at(index)) element--;
//
//	m_table.at(index) = 1 + 4 * BUFFER_SIZE;
//}
