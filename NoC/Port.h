#pragma once
#include "DataStructures.h"

class Port
{
public:
	Port() = default;
	Port(const PortType portType) : m_portType{ portType } {}
	virtual ~Port() = default;

	//bool isEmpty();
	virtual void receiveFlit() {};
	virtual void receiveCredit() {};

	// Input port
	FlitRegister m_inFlitRegister;
	CreditRegister m_outCreditRegister;
	FlitRegister m_crossbarInputRegister;

	// Output port
	FlitRegister m_outFlitRegister;
	CreditRegister m_inCreditRegister;

	// port type
	PortType m_portType{};

	// SA
	int m_virtualChannelSwitched{}; // default 0
	int m_switchPriorityGlobal{};
	PortType m_outputPortSwitched{}; // Unselected (default)

private:
};