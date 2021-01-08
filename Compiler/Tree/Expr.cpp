#include "Expr.h"
#include "AccessExpr.h"
#include "Class.h"

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
    node->Type = TypeT::ArrayNewWithArguments;
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

ExprNode* ExprNode::FromNew(StandardType standardType, ExprNode* expr)
{
    auto* node = new ExprNode;
    node->Type = TypeT::ArrayNew;
    auto dataType = ToDataType(standardType);
    dataType.ArrayArity += 1;
    node->NewArrayType = dataType;
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

    assign->AType = DataType::VoidType;

    return assign;
}

ExprNode* ExprNode::ToAssignOnField() const
{
    const auto isAssignmentOnField =
        Type == TypeT::Assign
        && Left->Type == TypeT::AccessExpr
        && Left->Access->ActualField != nullptr;
    if (!isAssignmentOnField)
        return nullptr;

    auto* expr = new ExprNode;
    expr->Type = TypeT::AssignOnField;
    expr->ObjectExpr = Left->Access->Previous;
    expr->AssignExpr = Right;
    expr->Field = Left->Access->ActualField;

    expr->AType = DataType::VoidType;

    return expr;
}

ExprNode* ExprNode::ToComplexArrayNew(std::vector<std::string>& errors) const
{
    const bool isArrayNew = Type == TypeT::SimpleNew
        && TypeNode->Access
        && TypeNode->Access->Type == AccessExpr::TypeT::ArrayElementExpr;

    if (!isArrayNew)
        return nullptr;

    const auto arrayType = ToDataType(TypeNode);

    if (isArrayNew && arrayType.IsUnknown)
    {
        errors.emplace_back("Cannot create such array");
        return nullptr;
    }

    auto* expr = new ExprNode;
    expr->Type = TypeT::ArrayNew;
    expr->NewArrayType = arrayType;
    expr->Child = TypeNode->Access->Child;
    return expr;
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

    if (ArrayExpr && ArrayExpr->Arguments)
    {
        for (auto& arg : ArrayExpr->Arguments->GetSeq())
            arg = mapFunction(arg);
    }

    if (IndexExpr)
        IndexExpr = mapFunction(IndexExpr);
    if (AssignExpr)
        AssignExpr = mapFunction(AssignExpr);

    if (ObjectExpr && ObjectExpr->Child)
        ObjectExpr->Child = mapFunction(ObjectExpr->Child);

    if (ObjectExpr && ObjectExpr->Arguments)
    {
        for (auto& arg : ObjectExpr->Arguments->GetSeq())
            arg = mapFunction(arg);
    }
}

void ExprNode::CallForAllChildren(const std::function<void(ExprNode*)>& function) const
{
    if (Left)
        function(Left);
    if (Right)
        function(Right);

    if (Child)
        function(Child);

    auto accessFunctor = [&function](AccessExpr* node)
    {
        if (node && node->Child)
            function(node->Child);
        if (node && node->Arguments)
            for (ExprNode* exprNode : node->Arguments->GetSeq()) { function(exprNode); }
    };

    accessFunctor(Access);
    accessFunctor(ArrayExpr);

    if (ExprSeq)
        for (auto& expr : ExprSeq->GetSeq())
            function(expr);

    if (IndexExpr)
        function(IndexExpr);
    if (AssignExpr)
        function(AssignExpr);
}
