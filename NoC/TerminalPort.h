#pragma once
#include "Port.h"

class TerminalPort : public Port
{
public:
	TerminalPort() = default;

	void runOneStep() override;



private:
};