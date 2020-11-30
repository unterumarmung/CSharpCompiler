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

inline std::string ToString(const StandardType type)
{
    switch (type)
    {
    case StandardType::Char:
        return "char";
    case StandardType::Int:
        return "int";
    case StandardType::Bool:
        return "bool";
    case StandardType::Float:
        return "float";
    case StandardType::String:
        return "string";
    }
    return {};
}

struct StandardArrayType
{
    StandardType Type{};
    size_t Arity{ 1 };
};

inline std::string ToString(const StandardArrayType type)
{
    auto res = ToString(type.Type);
    res.reserve(res.size() + type.Arity * 2);
    for (auto i = 0u; i < type.Arity; ++i) { res += "[]"; }
    return res;
}

struct TypeNode final : Node
{
    [[nodiscard]] std::string_view Name() const noexcept override { return "Type"; }

    enum class TypeT
    {
        StdType,
        StdArrType,
        AccessExpr
    } Type{};

    StandardType StdType{};
    StandardArrayType StdArrType{};
    AccessExpr* Access{};


    explicit TypeNode(const StandardType stdType)
        : StdType{ stdType }
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
