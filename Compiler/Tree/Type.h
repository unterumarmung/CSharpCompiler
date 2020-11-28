#pragma once
#include "Node.h"
#include "AccessExpr.h"

enum class StandardType
{
    Char,
    Int,
    Bool,
    Float,
    String
};

struct StandardArrayType
{
    StandardType Type{};
    size_t Arity{ 1 };
};

struct TypeNode : Node
{
    [[nodiscard]] std::string_view Name() const noexcept override { return "Type"; }

    enum class TypeT { StdType, StdArrType, AccessExpr } Type{};

    StandardType StdType{};
    StandardArrayType StdArrType{};
    AccessExpr* Access{};


    explicit TypeNode(const StandardType stdType)
        : Type{ TypeT::StdType }
        , StdType{ stdType }
    {
    }

    explicit TypeNode(const StandardArrayType stdArrType)
        : Type{ TypeT::StdArrType }
        , StdArrType{ stdArrType }
    {
    }

    explicit TypeNode(AccessExpr* const accessExpr)
        : Type{ TypeT::AccessExpr }
        , Access{ accessExpr }
    {
    }
};