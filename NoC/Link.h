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
		if (m_connection.first->m_outFlitRegister.empty())
			m_firstOutFlitEnable = false;
		else
			m_firstOutFlitEnable = true;

		if (m_connection.first->m_outCreditRegister.empty())
			m_firstOutCreditEnable = false;
		else
			m_firstOutCreditEnable = true;

		if (m_connection.second->m_outFlitRegister.empty())
			m_secondOutFlitEnable = false;
		else
			m_secondOutFlitEnable = true;

		if (m_connection.second->m_outCreditRegister.empty())
			m_secondOutCreditEnable = false;
		else
			m_secondOutCreditEnable = true;
	}

	Clock m_localClock{};
	std::pair<Port*, Port*> m_connection{};
	mutable bool m_firstOutFlitEnable{ false };
	mutable bool m_firstOutCreditEnable{ false };
	mutable bool m_secondOutFlitEnable{ false };
	mutable bool m_secondOutCreditEnable{ false };
private:
};