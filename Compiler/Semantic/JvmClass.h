#pragma once
#include <string>
#include <vector>
#include "../Tree/Class.h"

struct Field
{
    std::string Name;
    VisibilityModifier visibilityModifier;
};

struct Method
{
    std::string Name;
    VisibilityModifier visibilityModifier;
};

struct Class
{
    std::string Name;
    Class* Parent;
    std::vector<Method> Methods;
    std::vector<Field> Fields;
};

