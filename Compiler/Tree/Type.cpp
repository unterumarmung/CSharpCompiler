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
