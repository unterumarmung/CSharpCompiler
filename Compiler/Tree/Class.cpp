#include "Class.h"
#include "Program.h"

DataType ClassDeclNode::ToDataType() const
{
    DataType type{ DataType::TypeT::Complex };
    // if (Namespace) { type.ComplexType.emplace_back(Namespace->NamespaceName); }
    type.ComplexType.emplace_back(ClassName);
    return type;
}
