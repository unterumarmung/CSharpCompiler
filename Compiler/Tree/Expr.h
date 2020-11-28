#pragma once
#include <vector>
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
    explicit ExprNode() : Node() {}
};

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