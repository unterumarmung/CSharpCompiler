#include "Expr.h"
#include "AccessExpr.h"
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

ExprNode* ExprNode::FromCast(const StandardType standardType, ExprNode* expr)
{
    auto* node = new ExprNode;
    node->Type = TypeT::Cast;
    node->StandardTypeChild = standardType;
    node->Child = expr;
    return node;
}

ExprNode* ExprNode::ToAssignOnArrayElement() const
{
    const auto isAssignmentOnArrayElement =
        Type == TypeT::Assign
        && Left->Type == TypeT::AccessExpr
        && Left->Access->Type == AccessExpr::TypeT::ArrayElementExpr;
    if (!isAssignmentOnArrayElement)
        return nullptr;
    auto* assign = new ExprNode;
    assign->Type = TypeT::AssignOnArrayElement;
    assign->ArrayExpr = Left->Access->Previous;
    assign->IndexExpr = Left->Access->Child;
    assign->AssignExpr = Right;
    return assign;
}

void ExprNode::ApplyToAllChildren(const std::function<ExprNode*(ExprNode*)>& mapFunction)
{
    if (Left)
        Left = mapFunction(Left);
    if (Right)
        Right = mapFunction(Right);
    if (Access && Access->Child)
        Access->Child = mapFunction(Access->Child);

    if (ExprSeq)
        for (auto& expr : ExprSeq->GetSeq())
            expr = mapFunction(expr);

    if (ArrayExpr && ArrayExpr->Child)
        ArrayExpr->Child = mapFunction(ArrayExpr->Child);

    if (IndexExpr)
        IndexExpr = mapFunction(IndexExpr);
    if (AssignExpr)
        AssignExpr = mapFunction(AssignExpr);
}
