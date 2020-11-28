#pragma once
#include "Stmt.h"
#include "Type.h"

enum class VisibilityModifier
{
    Public,
    Protected,
    Private
};

struct FieldDeclNode final : Node
{
    const VisibilityModifier Visibility;
    const VarDeclNode* VarDecl;

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
    [[nodiscard]] std::string_view Name() const noexcept override
    {
        return "MethodArguments";
    }
};

struct MethodDeclNode final : Node
{
    const VisibilityModifier Visibility;
    const TypeNode* Type;
    const std::string_view Identifier;
    const MethodArguments* Arguments;
    const StmtSeqNode* Body;


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


    ClassDeclNode(const std::string_view className, IdentifierList* parentType, ClassMembersNode* const members)
        : ClassName{ className }
      , ParentType{ parentType }
      , Members{ members }
    {
    }

    [[nodiscard]] std::string_view Name() const noexcept override { return "ClassDecl"; }
};