#pragma once
#include "Node.h"

struct EnumDeclNode final : Node
{
    std::string_view EnumName;
    IdentifierList* Enumerators;

    EnumDeclNode(const std::string_view enumName, IdentifierList* const enumerators)
        : EnumName{ enumName }
        , Enumerators{ enumerators }
    {
    }

    [[nodiscard]] std::string_view Name() const noexcept override { return "EnumDecl"; }
};
