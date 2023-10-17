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
		if (m_inFlitRegister.empty()
			&& m_inCreditRegister.empty())
			m_enable = false;
		else
			m_enable = true;
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

	mutable bool m_enable{ false };

private:
};