#pragma once
#include "Port.h"
#include "Clock.h"
#include <utility>

class Link
{
public:
	Link(Port& inputPort, Port& outputPort) : m_connection{ &inputPort, &outputPort } {}

	bool operator<(const Link& link) const { return m_connection < link.m_connection; }
	void updateEnable() const
	{
		if (m_connection.first->m_outFlitRegister.empty()
			&& m_connection.first->m_outCreditRegister.empty()
			&& m_connection.second->m_outFlitRegister.empty()
			&& m_connection.second->m_outCreditRegister.empty())
			m_enable = false;
		else
			m_enable = true;
	}

	Clock m_localClock{};
	std::pair<Port*, Port*> m_connection{};
	mutable bool m_enable{ false };
private:
};