#pragma once

#include <ostream>
#include "Tree/Program.h"

void ToDot(Program* node, std::ostream& out);

void RunDot(std::string_view dotPath, std::string_view dotFilePath);
