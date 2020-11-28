#include "AccessExpr.h"

AccessExpr* AccessExpr::FromExpr(ExprNode* expr)
{
    auto* node = new AccessExpr;
    node->Type = TypeT::Expr;
    node->Child = expr;
    return node;
}

AccessExpr* AccessExpr::FromBrackets(AccessExpr* previous)
{
    auto* node = new AccessExpr;
    node->Type = TypeT::ComplexArrayType;
    node->Previous = previous;
    return node;
}

AccessExpr* AccessExpr::FromBrackets(AccessExpr* previous, ExprNode* child)
{
    auto* node = new AccessExpr;
    node->Type = TypeT::ArrayElementExpr;
    node->Previous = previous;
    node->Child = child;
    return node;
}

AccessExpr* AccessExpr::FromInt(int value)
{
    auto* node = new AccessExpr;
    node->Type = TypeT::Integer;
    node->Integer = value;
    return node;
}

AccessExpr* AccessExpr::FromFloat(double value)
{
    auto* node = new AccessExpr;
    node->Type = TypeT::Float;
    node->Float = value;
    return node;
}

AccessExpr* AccessExpr::FromString(const char* const str)
{
    auto* node = new AccessExpr;
    node->Type = TypeT::String;
    node->String = str;
    return node;
}

AccessExpr* AccessExpr::FromId(const char* const id)
{
    auto* node = new AccessExpr;
    node->Type = TypeT::Identifier;
    node->Identifier = id;
    return node;
}

AccessExpr* AccessExpr::FromBool(bool value)
{
    auto* node = new AccessExpr;
    node->Type = TypeT::Bool;
    node->Bool = value;
    return node;
}

AccessExpr* AccessExpr::FromChar(char value)
{
    auto* node = new AccessExpr;
    node->Type = TypeT::Char;
    node->Char = value;
    return node;
}

AccessExpr* AccessExpr::FromCall(const char* const id, ExprSeqNode* arguments)
{
    auto* node = new AccessExpr;
    node->Type = TypeT::SimpleMethodCall;
    node->Identifier = id;
    node->Arguments = arguments;
    return node;
}

AccessExpr* AccessExpr::FromDot(AccessExpr* previous, const char* const id)
{
    auto* node = new AccessExpr;
    node->Type = TypeT::Dot;
    node->Previous = previous;
    node->Identifier = id;
    return node;
}

AccessExpr* AccessExpr::FromDot(AccessExpr* previous, const char* const id, ExprSeqNode* arguments)
{
    auto* node = new AccessExpr;
    node->Type = TypeT::DotMethodCall;
    node->Previous = previous;
    node->Identifier = id;
    node->Arguments = arguments;
    return node;
}
