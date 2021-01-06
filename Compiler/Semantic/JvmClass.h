#pragma once
#include <string>
#include <vector>
#include "../VisibilityModified.h"

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

struct DataType
{
    enum class TypeT
    {
        Char = 0,
        Int = 1,
        Bool = 2,
        Float = 3,
        String = 4,
        Complex = 5,
        Void = 6
    } AType{};

    static const DataType VoidType;
    static const DataType BoolType;
    static const DataType IntType;

    int ArrayArity = 0;

    bool IsUnknown{};

    std::vector<std::string> ComplexType{};

    bool operator==(const DataType& data) const
    {
        return AType == data.AType
               && IsUnknown == data.IsUnknown
               && ArrayArity == data.ArrayArity
               && ComplexType == data.ComplexType;
    }

    bool operator!=(const DataType& data) const { return !(*this == data); }

    [[nodiscard]] std::string ToDescriptor() const
    {
        if (ArrayArity > 0)
        {
            auto subtype = *this;
            subtype.ArrayArity -= 1;
            return "[" + subtype.ToDescriptor();
        }
        switch (AType)
        {
        case TypeT::Char:
            return "C";
        case TypeT::Int:
            return "I";
        case TypeT::Bool:
            return "Z";
        case TypeT::Float:
            return "F";
        case TypeT::String:
            return "Ljava/lang/String;";
        case TypeT::Complex:
        {
            std::string value = "L";
            for (auto const& part : ComplexType)
            {
                value += part;
                value += '/';
            }
            value.back() = ';';
            return value;
        }
        case TypeT::Void:
            return "V";
        }
    }

    [[nodiscard]] std::string ToTypename() const
    {
        if (AType != TypeT::Complex)
            return ToDescriptor();

        std::string value;
        for (auto const& part : ComplexType)
        {
            value += part;
            value += '/';
        }
        value.pop_back();
        return value;
    }

    [[nodiscard]] bool IsReferenceType() const { return AType == TypeT::Complex || ArrayArity >= 1; }
};

inline std::string ToString(DataType data)
{
    if (data.IsUnknown)
        return "unknown";
    if (data.ArrayArity > 0)
    {
        auto temp = data;
        temp.ArrayArity = 0;
        auto str = ToString(temp);
        for (int i = 0; i < data.ArrayArity; ++i)
            str += "[]";
        return str;
    }
    switch (data.AType)
    {
    case DataType::TypeT::Char:
        return "char";
    case DataType::TypeT::Bool:
        return "bool";
    case DataType::TypeT::Int:
        return "int";
    case DataType::TypeT::Float:
        return "float";
    case DataType::TypeT::String:
        return "string";
    case DataType::TypeT::Complex:
    {
        std::string fullName;
        for (auto const& namePart : data.ComplexType)
        {
            fullName += namePart;
            fullName += '.';
        }
        fullName.pop_back();
        return fullName;
    }
    case DataType::TypeT::Void:
        return "void";
    }
}

inline std::string ToString(std::vector<DataType> const& container)
{
    std::string str;
    for (auto it = container.begin(); it != container.end(); ++it)
    {
        std::string_view suffix = ", ";
        if (it == std::prev(container.end()))
            suffix = "";

        str += ToString(*it);
        str += suffix;
    }
    return str;
}
