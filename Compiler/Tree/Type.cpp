#include "Type.h"

TypeNode::TypeNode(const StandardType stdType): StdType{ stdType }
{
}

TypeNode::TypeNode(const StandardArrayType stdArrType): Type{ TypeT::StdArrType }
                                                    , StdArrType{ stdArrType }
{
}

TypeNode::TypeNode(AccessExpr* const accessExpr): Type{ TypeT::AccessExpr }
                                              , Access{ accessExpr }
{
}

#include "AccessExpr.h"

DataType ToDataType(const TypeNode* node)
{
    if (!node)
        return { DataType::TypeT::Void };
    if (node->Type == TypeNode::TypeT::StdType) { return ToDataType(node->StdType); }
    if (node->Type == TypeNode::TypeT::StdArrType) { return ToDataType(node->StdArrType); }
    if (node->Type == TypeNode::TypeT::AccessExpr && node->Access) { return node->Access->ToDataType(); }
    return { DataType::TypeT::Void, {}, true };
}
