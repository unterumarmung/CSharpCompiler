#include "Expr.h"

ExprNode* ExprNode::FromBinaryExpression(TypeT type, ExprNode* lhs, ExprNode* rhs)
{
    auto* node = new ExprNode;
    node->Type = type;
    node->Left = lhs;
    node->Right = rhs;
    return node;
}

ExprNode* ExprNode::FromUnaryExpression(TypeT type, ExprNode* child)
{
    auto* node = new ExprNode;
    node->Type = type;
    node->Child = child;
    return node;
}

ExprNode* ExprNode::FromNull()
{
    auto* node = new ExprNode;
    node->Type = TypeT::Null;
    return node;
}

ExprNode* ExprNode::FromAccessExpr(AccessExpr* child)
{
    auto* node = new ExprNode;
    node->Type = TypeT::AccessExpr;
    node->Access = child;
    return node;
}

ExprNode* ExprNode::FromNew(::TypeNode* typeNode)
{
    auto* node = new ExprNode;
    node->Type = TypeT::SimpleNew;
    node->TypeNode = typeNode;
    return node;
}

ExprNode* ExprNode::FromNew(::TypeNode* typeNode, ExprSeqNode* exprSeq)
{
    auto* node = new ExprNode;
    node->Type = TypeT::ArrayNew;
    node->TypeNode = typeNode;
    node->ExprSeq = exprSeq;
    return node;
}
