#include "Dot.h"

#include <process.h>
#include <string>

using namespace std::string_literals;

void ToDot(AccessExpr* node, std::ostream& out, bool isType = false);

void ToDot(const TypeNode* node, std::ostream& out);

inline void ToDot(ExprNode* node, std::ostream& out);

void ToDot(const VarDeclNode* node, std::ostream& out);

void ToDot(WhileNode* node, std::ostream& out);

void ToDot(StmtNode* node, std::ostream& out);

void ToDot(const StmtSeqNode* node, std::ostream& out, Node* parent, bool markNext = true, bool createNode = false);

void ToDot(MethodDeclNode* node, std::ostream& out);

void ToDot(FieldDeclNode* node, std::ostream& out);

void ToDot(ClassDeclNode* node, std::ostream& out);

void ToDot(EnumDeclNode* node, std::ostream& out);

void ToDot(NamespaceDeclNode* node, std::ostream& out);

void ToDot(NamespaceDeclSeq* node, std::ostream& out);

void ToDot(UsingDirectiveNode* node, std::ostream& out);

void ToDot(UsingDirectives* node, std::ostream& out);

[[nodiscard]] std::string MakeNode(const std::size_t id, const std::string_view name, const std::string_view style = "")
{
    auto res = std::to_string(id) + " [label=\"" + std::string{ name } + "\"]";
    if (!style.empty()) { res += "[style = \"" + std::string{ style } + "\"]"; }
    res += "\n";
    return res;
}

[[nodiscard]] std::string MakeConnection(const size_t id1, const size_t id2, std::string_view note = "")
{
    auto res = std::to_string(id1) + " -> " + std::to_string(id2);

    if (!note.empty())
        res += " [label=\"" + std::string{ note } + "\"]";

    res += "\n";
    return res;
}

[[nodiscard]] std::string QualifiedNameToString(IdentifierList* identifiers)
{
    if (!identifiers)
        return "";
    std::string name;
    for (const auto& id : identifiers->Identifiers)
    {
        name += id;
        name += '.';
    }
    name.pop_back();
    return name;
}

void ToDot(ExprSeqNode* node, std::ostream& out, Node* parent, const bool markNext = true, std::string_view note = "")
{
    for (auto it = node->GetSeq().begin(); it != node->GetSeq().end(); ++it)
    {
        auto* expr = *it;
        ToDot(expr, out);
        out << MakeConnection(parent->Id, expr->Id, note);
        if (markNext)
        {
            const auto isFirst = expr->Id == node->GetSeq().front()->Id;
            if (!isFirst)
            {
                const auto previousId = (*(it - 1))->Id;
                auto connection = MakeConnection(previousId, expr->Id, "next");
                connection.pop_back();
                out << connection << "[style=\"dashed\"]\n";
            }
        }
    }
}

void ToDot(AccessExpr* const node, std::ostream& out, const bool isType)
{
    if (!node)
        return;
    const auto nameSuffix = isType
                                ? ""
                                : "\\nCalculated data type: " + ToString(node->AType);
    switch (node->Type)
    {
        case AccessExpr::TypeT::Expr:
            out << MakeNode(node->Id, "()" + nameSuffix);
            ToDot(node->Child, out);
            out << MakeConnection(node->Id, node->Child->Id);
            return;
        case AccessExpr::TypeT::ArrayElementExpr:
            out << MakeNode(node->Id, "Array Indexing" + nameSuffix);
            ToDot(node->Previous, out);
            out << MakeConnection(node->Id, node->Previous->Id, "array expr");
            ToDot(node->Child, out);
            out << MakeConnection(node->Id, node->Child->Id, "element expr");
            return;
        case AccessExpr::TypeT::ComplexArrayType:
            out << MakeNode(node->Id, "Complex Array Type" + nameSuffix);
            ToDot(node->Previous, out);
            out << MakeConnection(node->Id, node->Previous->Id);
            return;
        case AccessExpr::TypeT::Integer:
            out << MakeNode(node->Id, std::to_string(node->Integer) + nameSuffix);
            return;
        case AccessExpr::TypeT::Float:
            out << MakeNode(node->Id, std::to_string(node->Float) + nameSuffix);
            return;
        case AccessExpr::TypeT::String:
            out << MakeNode(node->Id, "\\\"" + std::string{ node->String } + "\\\"" + nameSuffix);
            return;
        case AccessExpr::TypeT::Char:
            out << MakeNode(node->Id, "\'" + std::string{ node->Char } + "\'" + nameSuffix);
            return;
        case AccessExpr::TypeT::Bool:
            out << MakeNode(node->Id, node->Bool ? "true" : "false" + nameSuffix);
            return;
        case AccessExpr::TypeT::Identifier:
            out << MakeNode(node->Id, std::string{ node->Identifier } + nameSuffix);
            return;
        case AccessExpr::TypeT::SimpleMethodCall:
            out << MakeNode(node->Id, "Method call\\nMethodName: " + std::string{ node->Identifier } + nameSuffix);
            ToDot(node->Arguments, out, node, true, "argument");
            if (node->ActualMethodCall) { out << MakeConnection(node->Id, node->ActualMethodCall->Id, "calls"); }
            return;
        case AccessExpr::TypeT::Dot:
            out << MakeNode(node->Id, "." + std::string{ node->Identifier } + nameSuffix);
            ToDot(node->Previous, out);
            out << MakeConnection(node->Id, node->Previous->Id);
            return;
        case AccessExpr::TypeT::DotMethodCall:
            out << MakeNode(node->Id, "Method call\\nMethodName: " + std::string{ node->Identifier } + nameSuffix);
            ToDot(node->Previous, out);
            out << MakeConnection(node->Id, node->Previous->Id, "call expr");
            ToDot(node->Arguments, out, node, true, "argument");
        case AccessExpr::TypeT::ArrayLength:
            out << MakeNode(node->Id, "Get Array Length");
            ToDot(node->Previous, out);
            out << MakeConnection(node->Id, node->Previous->Id);
        default: ;
    }
}

void ToDot(const TypeNode* node, std::ostream& out)
{
    std::string name{ node->Name() };

    switch (node->Type)
    {
        case TypeNode::TypeT::StdType:
            name += "\\n" + ToString(node->StdType);
            out << MakeNode(node->Id, name);
            return;
        case TypeNode::TypeT::StdArrType:
            name += "\\n" + ToString(node->StdArrType);
            out << MakeNode(node->Id, name);
            return;
        case TypeNode::TypeT::AccessExpr:
            out << MakeNode(node->Id, name);
            ToDot(node->Access, out, true);
            out << MakeConnection(node->Id, node->Access->Id, "Complex type");
    }
}


void ToDot(ExprNode* const node, std::ostream& out)
{
    if (node->Type == ExprNode::TypeT::AccessExpr)
    {
        out << MakeNode(node->Id, "AccessExpr");
        ToDot(node->Access, out);
        out << MakeConnection(node->Id, node->Access->Id);
        return;
    }

    auto name = node->Type == ExprNode::TypeT::Cast
                    ? "Cast to " + ToString(node->StandardTypeChild)
                    : ToString(node->Type);

    name += "\nCalculated data type: "s + ToString(node->AType);

    out << MakeNode(node->Id, name);

    if (node->Type == ExprNode::TypeT::ArrayNew)
    {
        ToDot(node->Child, out);
        out << MakeConnection(node->Id, node->Child->Id, "Array size");
    }

    if (IsBinary(node->Type))
    {
        ToDot(node->Left, out);
        ToDot(node->Right, out);
        out << MakeConnection(node->Id, node->Left->Id, "left operand");
        out << MakeConnection(node->Id, node->Right->Id, "right operand");
        return;
    }
    if (IsUnary(node->Type))
    {
        ToDot(node->Child, out);
        out << MakeConnection(node->Id, node->Child->Id);
        return;
    }
    if (node->Type == ExprNode::TypeT::SimpleNew)
    {
        ToDot(node->TypeNode, out);
        out << MakeConnection(node->Id, node->TypeNode->Id, "type");
    }
    if (node->Type == ExprNode::TypeT::ArrayNewWithArguments)
    {
        ToDot(node->TypeNode, out);
        out << MakeConnection(node->Id, node->TypeNode->Id, "type");
        ToDot(node->ExprSeq, out, node);
    }

    if (node->Type == ExprNode::TypeT::AssignOnArrayElement)
    {
        ToDot(node->ArrayExpr, out);
        ToDot(node->IndexExpr, out);
        ToDot(node->AssignExpr, out);
        out << MakeConnection(node->Id, node->ArrayExpr->Id, "array");
        out << MakeConnection(node->Id, node->IndexExpr->Id, "index");
        out << MakeConnection(node->Id, node->AssignExpr->Id, "assign");
    }

    if (node->Type == ExprNode::TypeT::AssignOnField)
    {
        if (node->ObjectExpr)
            ToDot(node->ObjectExpr, out);
        ToDot(node->AssignExpr, out);
        if (node->ObjectExpr)
            out << MakeConnection(node->Id, node->ObjectExpr->Id, "object");
        out << MakeConnection(node->Id, node->AssignExpr->Id, "assign");
    }
}

void ToDot(const VarDeclNode* node, std::ostream& out)
{
    auto name = std::string{ node->Name() } + "\\nName = "s + std::string{ node->Identifier };
    name += "\nCalculated data type: " + ToString(node->AType);
    out << MakeNode(node->Id, name);
    if (node->VarType != nullptr)
    {
        ToDot(node->VarType, out);
        out << MakeConnection(node->Id, node->VarType->Id, "Variable type");
    }
    if (node->InitExpr != nullptr)
    {
        ToDot(node->InitExpr, out);
        out << MakeConnection(node->Id, node->InitExpr->Id, "Init");
    }
}

void ToDot(WhileNode* node, std::ostream& out)
{
    out << MakeNode(node->Id, node->Name());
    ToDot(node->Condition, out);
    out << MakeConnection(node->Id, node->Condition->Id, "Condition");
    ToDot(node->Body, out);
    out << MakeConnection(node->Id, node->Body->Id, "Body");
}

void ToDot(DoWhileNode* node, std::ostream& out)
{
    out << MakeNode(node->Id, node->Name());
    ToDot(node->Condition, out);
    out << MakeConnection(node->Id, node->Condition->Id, "Condition");
    ToDot(node->Body, out);
    out << MakeConnection(node->Id, node->Body->Id, "Body");
}

void ToDot(ForNode* node, std::ostream& out)
{
    out << MakeNode(node->Id, node->Name());

    if (node->VarDecl)
    {
        ToDot(node->VarDecl, out);
        out << MakeConnection(node->Id, node->VarDecl->Id, "Loop variable");
    }
    if (node->FirstExpr)
    {
        ToDot(node->FirstExpr, out);
        out << MakeConnection(node->Id, node->FirstExpr->Id, "First expression");
    }
    if (node->Condition)
    {
        ToDot(node->Condition, out);
        out << MakeConnection(node->Id, node->Condition->Id, "Condition");
    }
    if (node->IterExpr)
    {
        ToDot(node->IterExpr, out);
        out << MakeConnection(node->Id, node->IterExpr->Id, "IterExpr");
    }

    ToDot(node->Body, out);
    out << MakeConnection(node->Id, node->Body->Id, "Body");
}

void ToDot(ForEachNode* node, std::ostream& out)
{
    out << MakeNode(node->Id, node->Name());

    ToDot(node->VarDecl, out);
    out << MakeConnection(node->Id, node->VarDecl->Id, "Loop variable");

    ToDot(node->Expr, out);
    out << MakeConnection(node->Id, node->Expr->Id, "Iter Expr");

    ToDot(node->Body, out);
    out << MakeConnection(node->Id, node->Body->Id, "Body");
}

void ToDot(IfNode* node, std::ostream& out)
{
    out << MakeNode(node->Id, node->Name());

    ToDot(node->Condition, out);
    out << MakeConnection(node->Id, node->Condition->Id, "Condition");

    ToDot(node->ThenBranch, out);
    out << MakeConnection(node->Id, node->ThenBranch->Id, "Then Branch");

    if (node->ElseBranch)
    {
        ToDot(node->ElseBranch, out);
        out << MakeConnection(node->Id, node->ElseBranch->Id, "Else Branch");
    }
}

void ToDot(StmtNode* node, std::ostream& out)
{
    switch (node->Type)
    {
        case StmtNode::TypeT::Empty:
            out << MakeNode(node->Id, "<<EMPTY STATEMENT>>");
            return;
        case StmtNode::TypeT::VarDecl:
            out << MakeNode(node->Id, "STMT");
            ToDot(node->VarDecl, out);
            out << MakeConnection(node->Id, node->VarDecl->Id);
            return;
        case StmtNode::TypeT::While:
            out << MakeNode(node->Id, "STMT");
            ToDot(node->While, out);
            out << MakeConnection(node->Id, node->While->Id);
            return;
        case StmtNode::TypeT::DoWhile:
            out << MakeNode(node->Id, "STMT");
            ToDot(node->DoWhile, out);
            out << MakeConnection(node->Id, node->DoWhile->Id);
            return;
        case StmtNode::TypeT::For:
            out << MakeNode(node->Id, "STMT");
            ToDot(node->For, out);
            out << MakeConnection(node->Id, node->For->Id);
            return;
        case StmtNode::TypeT::Foreach:
            out << MakeNode(node->Id, "STMT");
            ToDot(node->ForEach, out);
            out << MakeConnection(node->Id, node->ForEach->Id);
            return;
        case StmtNode::TypeT::BlockStmt:
            out << MakeNode(node->Id, "Block Statement");
            return ToDot(node->Block, out, node);
        case StmtNode::TypeT::IfStmt:
            out << MakeNode(node->Id, "STMT");
            ToDot(node->If, out);
            out << MakeConnection(node->Id, node->If->Id);
            return;
        case StmtNode::TypeT::Return:
            out << MakeNode(node->Id, "Return");
            ToDot(node->Expr, out);
            out << MakeConnection(node->Id, node->Expr->Id);
            return;
        case StmtNode::TypeT::ExprStmt:
            out << MakeNode(node->Id, "Expr statement");
            ToDot(node->Expr, out);
            out << MakeConnection(node->Id, node->Expr->Id);
    }
}

void ToDot(const StmtSeqNode* node, std::ostream& out, Node* parent, const bool markNext, const bool createNode)
{
    if (!node)
        return;
    size_t parentId = parent->Id;
    if (createNode)
    {
        out << MakeNode(node->Id, "Stmt Seq");
        parentId = node->Id;
    }
    for (auto it = node->GetSeq().begin(); it != node->GetSeq().end(); ++it)
    {
        auto* stmt = *it;
        ToDot(stmt, out);
        out << MakeConnection(parentId, stmt->Id);
        if (markNext)
        {
            const auto isFirst = stmt->Id == node->GetSeq().front()->Id;
            if (!isFirst)
            {
                const auto previousId = (*(it - 1))->Id;
                auto connection = MakeConnection(previousId, stmt->Id, "next stmt");
                connection.pop_back();
                out << connection << "[style=\"dashed\"]\n";
            }
        }
    }
}

void ToDot(MethodDeclNode* node, std::ostream& out)
{
    const auto name = std::string{ node->Name() }
                      + "\\nVisibility: " + std::string{ ToString(node->Visibility) }
                      + "\\nName: " + node->Identifier();
    out << MakeNode(node->Id, name);

    if (node->Type)
    {
        ToDot(node->Type, out);
        out << MakeConnection(node->Id, node->Type->Id, "Return value type");
    }
    for (auto* arg : node->Arguments->GetSeq())
    {
        ToDot(arg, out);
        out << MakeConnection(node->Id, arg->Id, "Method argument");
    }

    if (node->Body)
    {
        ToDot(node->Body, out, node, true, true);
        out << MakeConnection(node->Id, node->Body->Id, "Method body");
    }
}

void ToDot(FieldDeclNode* node, std::ostream& out)
{
    const auto name = std::string{ node->Name() } + "\\nVisibility: " + std::string{ ToString(node->Visibility) };
    out << MakeNode(node->Id, name);
    ToDot(node->VarDecl, out);
    out << MakeConnection(node->Id, node->VarDecl->Id, "Field Variable");
}

void ToDot(ClassDeclNode* node, std::ostream& out)
{
    auto name = std::string{ node->Name() } + "\\n" + std::string{ node->ClassName };
    name += "\\n Inherited from: " + QualifiedNameToString(node->ParentType);
    out << MakeNode(node->Id, name);

    for (auto* method : node->Members->Methods)
    {
        ToDot(method, out);
        out << MakeConnection(node->Id, method->Id);
    }

    for (auto* field : node->Members->Fields)
    {
        ToDot(field, out);
        out << MakeConnection(node->Id, field->Id);
    }
}

void ToDot(EnumDeclNode* node, std::ostream& out)
{
    auto name = std::string{ node->Name() } + "\\n" + std::string{ node->EnumName } + "\\n";
    for (const auto& id : node->Enumerators->Identifiers)
    {
        name += "\\n";
        name += id;
    }

    out << MakeNode(node->Id, name);
}

void ToDot(NamespaceDeclNode* node, std::ostream& out)
{
    const auto name = std::string{ node->Name() } + "\\n" + std::string{ node->NamespaceName };
    out << MakeNode(node->Id, name);
    for (const auto& class_ : node->Members->Classes)
    {
        ToDot(class_, out);
        out << MakeConnection(node->Id, class_->Id);
    }

    for (const auto& enum_ : node->Members->Enums)
    {
        ToDot(enum_, out);
        out << MakeConnection(node->Id, enum_->Id);
    }
}

void ToDot(NamespaceDeclSeq* node, std::ostream& out)
{
    out << MakeNode(node->Id, node->Name());
    for (const auto& child : node->GetSeq())
    {
        ToDot(child, out);
        out << MakeConnection(node->Id, child->Id);
    }
}

void ToDot(UsingDirectiveNode* node, std::ostream& out)
{
    const auto name = std::string{ node->Name() } + " " + QualifiedNameToString(node->Arg);

    out << MakeNode(node->Id, name);
}

void ToDot(UsingDirectives* node, std::ostream& out)
{
    out << MakeNode(node->Id, node->Name());
    for (const auto& child : node->GetSeq())
    {
        ToDot(child, out);
        out << MakeConnection(node->Id, child->Id);
    }
}

void ToDot(Program* node, std::ostream& out)
{
    out << "digraph Program {\n";
    out << MakeNode(node->Id, node->Name());
    ToDot(node->Usings, out);
    ToDot(node->Namespaces, out);
    out << MakeConnection(node->Id, node->Usings->Id);
    out << MakeConnection(node->Id, node->Namespaces->Id);
    out << "}" << std::endl;
}

void RunDot(const std::string_view dotPath, const std::string_view dotFilePath)
{
    _spawnl(_P_NOWAIT, dotPath.data(), "dot", "-O", "-Tsvg", dotFilePath.data(), nullptr);
}
