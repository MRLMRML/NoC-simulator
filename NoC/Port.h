#pragma once
#include "DataStructures.h"

class Port
{
public:
	Port() = default;
	virtual ~Port() = default;

	bool isEmpty();

	// Input port
	std::deque<Flit> m_outFlitBuffer;
	std::deque<Credit> m_inCreditBuffer;

	// Output port
	std::deque<Flit> m_inFlitBuffer;
	std::deque<Credit> m_outCreditBuffer;


private:
};