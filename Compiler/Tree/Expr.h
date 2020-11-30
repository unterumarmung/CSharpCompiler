#pragma once
#include "Node.h"

struct AccessExpr;
struct ExprSeqNode;
struct TypeNode;

struct ExprNode final : Node
{
    enum class TypeT
    {
        BinPlus,
        BinMinus,
        Multiply,
        Divide,
        Assign,
        Less,
        Greater,
        Equal,
        NotEqual,
        GreaterOrEqual,
        LessOrEqual,
        And,
        Or,
        Not,
        UnaryMinus,
        Null,
        AccessExpr,
        SimpleNew,
        ArrayNew
    } Type{};

    ExprNode* Left{};
    ExprNode* Right{};

    ExprNode* Child{};

    AccessExpr* Access{};

    ExprSeqNode* ExprSeq{};

    TypeNode* TypeNode{};

    static ExprNode* FromBinaryExpression(TypeT type, ExprNode* lhs, ExprNode* rhs);

    static ExprNode* FromUnaryExpression(TypeT type, ExprNode* child);

    static ExprNode* FromNull();

    static ExprNode* FromAccessExpr(AccessExpr* child);

    static ExprNode* FromNew(struct TypeNode* typeNode);

    static ExprNode* FromNew(struct TypeNode* typeNode, ExprSeqNode* exprSeq);

    [[nodiscard]] std::string_view Name() const noexcept override { return "Expr"; }

private:
    explicit ExprNode() : Node()
    {
    }
};

inline bool IsBinary(const ExprNode::TypeT type)
{
    switch (type)
    {
        // NOLINT(clang-diagnostic-switch-enum)
    case ExprNode::TypeT::BinPlus:
    case ExprNode::TypeT::BinMinus:
    case ExprNode::TypeT::Multiply:
    case ExprNode::TypeT::Divide:
    case ExprNode::TypeT::Assign:
    case ExprNode::TypeT::Less:
    case ExprNode::TypeT::Greater:
    case ExprNode::TypeT::Equal:
    case ExprNode::TypeT::NotEqual:
    case ExprNode::TypeT::GreaterOrEqual:
    case ExprNode::TypeT::LessOrEqual:
    case ExprNode::TypeT::And:
    case ExprNode::TypeT::Or:
        return true;
    default:
        return false;
    }
}

inline bool IsUnary(const ExprNode::TypeT type)
{
    switch (type)
    {
        // NOLINT(clang-diagnostic-switch-enum)
    case ExprNode::TypeT::Not:
    case ExprNode::TypeT::UnaryMinus:
        return true;
    default:
        return false;
    }
}

inline std::string_view ToString(const ExprNode::TypeT type)
{
    switch (type)
    {
    case ExprNode::TypeT::BinPlus:
        return "+";
    case ExprNode::TypeT::BinMinus:
        return "-";
    case ExprNode::TypeT::Multiply:
        return "*";
    case ExprNode::TypeT::Divide:
        return "/";
    case ExprNode::TypeT::Assign:
        return "=";
    case ExprNode::TypeT::Less:
        return "<";
    case ExprNode::TypeT::Greater:
        return ">";
    case ExprNode::TypeT::Equal:
        return "==";
    case ExprNode::TypeT::NotEqual:
        return "!=";
    case ExprNode::TypeT::GreaterOrEqual:
        return ">=";
    case ExprNode::TypeT::LessOrEqual:
        return "<=";
    case ExprNode::TypeT::And:
        return "&&";
    case ExprNode::TypeT::Or:
        return "||";
    case ExprNode::TypeT::Not:
        return "!";
    case ExprNode::TypeT::UnaryMinus:
        return "-";
    case ExprNode::TypeT::Null:
        return "null";
    case ExprNode::TypeT::SimpleNew:
        return "new";
    case ExprNode::TypeT::ArrayNew:
        return "new[]";
    case ExprNode::TypeT::AccessExpr:
        return "AccessExpr";
    default:
        return "";
    }
}

struct ExprSeqNode final : NodeSeq<ExprSeqNode, ExprNode>
{
    using NodeSeq<ExprSeqNode, ExprNode>::NodeSeq;

    [[nodiscard]] std::string_view Name() const noexcept override
    {
        if (IsEmpty())
            return "Empty ExprSeq";
        return "ExprSeq";
    }
};
