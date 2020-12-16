#pragma once
#include <vector>
#include "JvmClass.h"
#include "../Tree/Program.h"

struct Semantic
{
    // ReSharper disable once CppInconsistentNaming
    Program* program;
    std::vector<JvmClass> Classes;

    explicit Semantic(Program* program) : program{ program }
    {
        
    }

    bool Analyze();
};

