#pragma once
#include "Node.h"
#include "../Semantic/JvmClass.h"

struct AccessExpr;

enum class StandardType
{
    Char = 0,
    Int = 1,
    Bool = 2,
    Float = 3,
    String = 4
};

inline DataType ToDataType(StandardType standard)
{
    switch (standard)
    {
    case StandardType::Char:
        return { DataType::TypeT::Char, false };
    case StandardType::Int:
        return { DataType::TypeT::Int, false };
    case StandardType::Bool:
        return { DataType::TypeT::Bool, false };
    case StandardType::Float:
        return { DataType::TypeT::Float, false };
    case StandardType::String:
        return { DataType::TypeT::String, false };
    default: ;
    }
    return {};
}

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

inline DataType ToDataType(StandardArrayType type)
{
    DataType data = ToDataType(type.Type);
    data.ArrayArity = type.Arity;
    return data;
}

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

    explicit TypeNode(StandardType stdType);

    explicit TypeNode(StandardArrayType stdArrType);

    explicit TypeNode(AccessExpr* accessExpr);
};

[[nodiscard]] DataType ToDataType(const TypeNode* node);
