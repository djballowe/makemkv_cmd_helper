#pragma once

#include "parse_output.h"
#include <future>
#include <string>
#include <vector>

std::string buildRipCommand(const char *command, const std::string destination, std::atomic<bool> &finished);
