#pragma once

#include <string>

enum class VisibilityModifier
{
    Public,
    Protected,
    Private
};

inline std::string ToString(VisibilityModifier e)
{
    switch (e)
    {
        case VisibilityModifier::Public:
            return "public";
        case VisibilityModifier::Protected:
            return "protected";
        case VisibilityModifier::Private:
            return "private";
        default:
            return "unknown";
    }
}
