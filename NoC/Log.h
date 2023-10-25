#pragma once
#include "Parameters.h"
#include <string_view>
#include <iostream>

void log(std::string_view string);

void logDebug(std::string_view string);

void log(const DATA_PRECISION& value);

void log(const size_t& value);

void log(const int& value);

void log(const long& value);

