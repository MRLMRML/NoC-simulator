#pragma once
#include "DataStructures.h"
#include "Port.h"

class Router
{
public:
	Router() = default;

public:
	Port m_northPort{};
	Port m_southPort{};
	Port m_westPort{};
	Port m_eastPort{};
	Port m_terminalPort{};

private:
};