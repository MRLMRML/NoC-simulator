#pragma once
#include "Port.h"
#include "Clock.h"
#include <utility>

class Link
{
public:
	Link(Port& inputPort, Port& outputPort) : m_connection{ &inputPort, &outputPort } {}

	Clock m_localClock{};
	std::pair<Port*, Port*> m_connection{};

	bool operator<(const Link& link) const
	{
		return m_connection < link.m_connection;
	}
private:
};