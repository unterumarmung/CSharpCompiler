#pragma once
#include "Stmt.h"
#include "Type.h"
#include "../VisibilityModified.h"


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

struct MethodDeclNode final : Node
{
    const VisibilityModifier Visibility;
    const TypeNode* Type;
    const std::string_view Identifier;
    const MethodArguments* Arguments;
    const StmtSeqNode* Body;
    std::vector<VarDeclNode*> variables{};

    VarDeclNode* FindVariableByName(std::string_view var)
    {
        for (auto* variable : variables)
        {
            if (variable->Identifier == var)
                return variable;
        }
        return nullptr;
    }

    MethodDeclNode(const VisibilityModifier visibility, const TypeNode* const type, const std::string_view identifier,
                   const MethodArguments* const arguments, const StmtSeqNode* const body)
        : Visibility{ visibility }
      , Type{ type }
      , Identifier{ identifier }
      , Arguments{ arguments }
      , Body{ body }
    {
    }

    [[nodiscard]] std::string_view Name() const noexcept override { return "MethodDecl"; }
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

struct ClassDeclNode final : Node
{
    std::string_view ClassName;
    IdentifierList* ParentType;
    ClassMembersNode* Members;

    FieldDeclNode* FindFieldByName(std::string_view name)
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
