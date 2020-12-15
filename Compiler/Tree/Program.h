#pragma once
#include "Class.h"
#include "Enum.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "AccessExpr.h"

struct NamespaceMembersNode final : Node
{
    std::vector<ClassDeclNode*> Classes;
    std::vector<EnumDeclNode*> Enums;

    void Add(ClassDeclNode* node) { Classes.push_back(node); }
    void Add(EnumDeclNode* node) { Enums.push_back(node); }

    NamespaceMembersNode() noexcept = default;

    [[nodiscard]] std::string_view Name() const noexcept override { return "NamespaceMembers"; }
};

struct NamespaceDeclNode final : Node
{
    std::string_view NamespaceName;
    NamespaceMembersNode* Members;


    NamespaceDeclNode(const std::string_view namespaceName, NamespaceMembersNode* const members)
        : NamespaceName{ namespaceName }
      , Members{ members }
    {
    }

    [[nodiscard]] std::string_view Name() const noexcept override { return "NamespaceDecl"; }
};

struct UsingDirectiveNode final : Node
{
    IdentifierList* Arg;

    explicit UsingDirectiveNode(IdentifierList* const arg)
        : Arg{ arg }
    {
    }

    [[nodiscard]] std::string_view Name() const noexcept override { return "Using"; }
};

struct UsingDirectives final : NodeSeq<UsingDirectives, UsingDirectiveNode>
{
    using NodeSeq<UsingDirectives, UsingDirectiveNode>::NodeSeq;

    [[nodiscard]] std::string_view Name() const noexcept override { return "UsingDirectives"; }
};

struct NamespaceDeclSeq final : NodeSeq<NamespaceDeclSeq, NamespaceDeclNode>
{
    using NodeSeq<NamespaceDeclSeq, NamespaceDeclNode>::NodeSeq;

    [[nodiscard]] std::string_view Name() const noexcept override { return "Namespace Declarations"; }
};

struct Program final : Node
{
    UsingDirectives* Usings;
    NamespaceDeclSeq* Namespaces;

    Program(UsingDirectives* const usings, NamespaceDeclSeq* const namespaces)
        : Usings{ usings }
      , Namespaces{ namespaces }
    {
    }

    [[nodiscard]] std::string_view Name() const noexcept override { return "Program"; }
};
