#pragma once
#include "DataStructures.h"

class Port
{
public:
	Port() = default;
	Port(const PortType portType) : m_portType{ portType } {}
	virtual ~Port() = default;

	void updateEnable() const
	{
		if (m_inFlitRegister.empty())
			m_inFlitEnable = false;
		else
			m_inFlitEnable = true;

		if (m_inCreditRegister.empty())
			m_inCreditEnable = false;
		else
			m_inCreditEnable = true;
	}

	//bool isEmpty();
	virtual void receiveFlit() {};

	// Input port
	std::deque<Flit> m_inFlitRegister;
	std::deque<Credit> m_outCreditRegister;
	std::deque<Flit> m_crossbarInputRegister;

	// Output port
	std::deque<Flit> m_outFlitRegister;
	std::deque<Credit> m_inCreditRegister;

	// port type
	PortType m_portType{};

	// SA
	int m_virtualChannelSwitched{}; // default 0
	int m_switchPriorityGlobal{};
	PortType m_outputPortSwitched{}; // Unselected (default)

	mutable bool m_inFlitEnable{ false };
	mutable bool m_inCreditEnable{ false };

private:
};