#pragma once
#include <algorithm>
#include <iterator>


#include "Stmt.h"
#include "Type.h"
#include "../VisibilityModified.h"


struct ClassDeclNode;

inline std::string_view ToString(VisibilityModifier modifier)
{
    switch (modifier)
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

struct FieldDeclNode final : Node
{
    const VisibilityModifier Visibility;
    VarDeclNode* VarDecl;
    ClassDeclNode* Class{};

    FieldDeclNode(const VisibilityModifier visibility, VarDeclNode* const varDecl)
        : Visibility{ visibility }
      , VarDecl{ varDecl }
    {
    }


    [[nodiscard]] std::string_view Name() const noexcept override { return "FieldDeclNode"; }
};

struct MethodArguments final : NodeSeq<MethodArguments, VarDeclNode>
{
    using NodeSeq<MethodArguments, VarDeclNode>::NodeSeq;
    [[nodiscard]] std::string_view Name() const noexcept override { return "MethodArguments"; }
};

struct MethodArgumentDto
{
    DataType Type;
    std::string Name;
};

inline std::vector<DataType> ToTypes(std::vector<MethodArgumentDto> const& arguments)
{
    std::vector<DataType> types(arguments.size());
    std::transform(arguments.begin(), arguments.end(), types.begin(), [](auto& arg) { return arg.Type; });
    return types;
}

inline MethodArgumentDto ToMethodArgumentDto(VarDeclNode* node)
{
    return {
        ToDataType(node->VarType),
        std::string{ node->Identifier }
    };
}

struct MethodDeclNode final : Node
{
    const VisibilityModifier Visibility;
    const TypeNode* Type;
    std::string_view Identifier;
    const MethodArguments* Arguments;
    const StmtSeqNode* Body;
    const bool IsStatic;
    const bool IsConstructor;
    std::vector<VarDeclNode*> Variables{};
    DataType AReturnType{};
    std::vector<MethodArgumentDto> ArgumentDtos{};

    ClassDeclNode* Class{};

    VarDeclNode* FindVariableByName(std::string_view var)
    {
        for (auto* variable : Variables)
        {
            if (variable->Identifier == var)
                return variable;
        }
        return nullptr;
    }

    MethodDeclNode(const VisibilityModifier visibility, const TypeNode* const type, const std::string_view identifier,
                   const MethodArguments* const arguments, const StmtSeqNode* const body, const bool isStatic = false)
        : Visibility{ visibility }
      , Type{ type }
      , Identifier{ identifier }
      , Arguments{ arguments }
      , Body{ body }
      , IsStatic{ isStatic }
      , IsConstructor{ Identifier == "<init>" }
    {
    }

    [[nodiscard]] std::string_view Name() const noexcept override { return "MethodDecl"; }

    [[nodiscard]] std::string ToDescriptor() const
    {
        std::string desc = "(";
        for (const auto& param : ArgumentDtos) { desc += param.Type.ToDescriptor(); }
        desc += ")";
        desc += AReturnType.ToDescriptor();
        return desc;
    }

    void AnalyzeArguments()
    {
        auto&& varDeclNodes = Arguments->GetSeq();
        std::transform(varDeclNodes.begin(), varDeclNodes.end(),
                       std::back_inserter(ArgumentDtos),
                       ToMethodArgumentDto);
    }
};

struct ClassMembersNode final : Node
{
    std::vector<MethodDeclNode*> Methods;
    std::vector<FieldDeclNode*> Fields;

    void Add(MethodDeclNode* node) { Methods.push_back(node); }
    void Add(FieldDeclNode* node) { Fields.push_back(node); }

    ClassMembersNode() noexcept = default;

    [[nodiscard]] std::string_view Name() const noexcept override { return "ClassMembers"; }
};

struct NamespaceDeclNode;

struct ClassDeclNode final : Node
{
    std::string_view ClassName;
    IdentifierList* ParentType;
    ClassMembersNode* Members;
    NamespaceDeclNode* Namespace{};

    [[nodiscard]] DataType ToDataType() const;

    [[nodiscard]] FieldDeclNode* FindFieldByName(std::string_view name) const
    {
        for (auto* field : Members->Fields)
        {
            if (field->VarDecl->Identifier == name)
                return field;
        }
        return nullptr;
    }


    ClassDeclNode(const std::string_view className, IdentifierList* parentType, ClassMembersNode* const members)
        : ClassName{ className }
      , ParentType{ parentType }
      , Members{ members }
    {
    }

    [[nodiscard]] std::string_view Name() const noexcept override { return "ClassDecl"; }
};
