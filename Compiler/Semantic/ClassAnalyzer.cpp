#include "ClassAnalyzer.h"

bool operator==(const Constant& lhs, const Constant& rhs)
{
    if (lhs.Type != rhs.Type) { return false; }
    switch (lhs.Type)
    {
    case Constant::TypeT::Utf8:
        return lhs.Utf8 == rhs.Utf8;
    case Constant::TypeT::Integer:
        return lhs.Integer == rhs.Integer;
    case Constant::TypeT::Float:
        return lhs.Float == rhs.Float;
    case Constant::TypeT::String:
    case Constant::TypeT::Class:
        return lhs.Utf8Id == rhs.Utf8Id;
    case Constant::TypeT::NameAndType:
        return lhs.NameId == rhs.NameId && lhs.TypeId == rhs.TypeId;
    case Constant::TypeT::MethodRef:
    case Constant::TypeT::FieldRef:
        return lhs.NameAndTypeId == rhs.NameAndTypeId && lhs.ClassId == rhs.ClassId;
    }
    return false;
}

Constant Constant::CreateUtf8(std::string const& content)
{
    Constant constant;
    constant.Type = TypeT::Utf8;
    constant.Utf8 = content;
    return constant;
}

Constant Constant::CreateInt(IntT i)
{
    Constant constant;
    constant.Type = TypeT::Integer;
    constant.Integer = i;
    return constant;
}

Constant Constant::CreateFloat(FloatT float_)
{
    Constant constant;
    constant.Type = TypeT::Float;
    constant.Float = float_;
    return constant;
}

Constant Constant::CreateString(IdT Utf8)
{
    Constant constant;
    constant.Type = TypeT::String;
    constant.Utf8Id = Utf8;
    return constant;
}

Constant Constant::CreateNaT(IdT nameId, IdT typeId)
{
    Constant constant;
    constant.Type = TypeT::NameAndType;
    constant.NameId = nameId;
    constant.TypeId = typeId;
    return constant;
}

Constant Constant::CreateClass(IdT classNameId)
{
    Constant constant;
    constant.Type = TypeT::Class;
    constant.ClassNameId = classNameId;
    return constant;
}

Constant Constant::CreateFieldRef(IdT natId, IdT classId)
{
    Constant constant;
    constant.Type = TypeT::FieldRef;
    constant.NameAndTypeId = natId;
    constant.ClassId = classId;
    return constant;
}

Constant Constant::CreateMethodRef(IdT natId, IdT classId)
{
    Constant constant;
    constant.Type = TypeT::MethodRef;
    constant.NameAndTypeId = natId;
    constant.ClassId = classId;
    return constant;
}

IdT ConstantTable::FindUtf8(std::string const& utf8)
{
    const auto constant = Constant::CreateUtf8(utf8);
    const auto foundIter = std::find(Constants.begin(), Constants.end(), constant);
    if (foundIter == Constants.end())
    {
        Constants.push_back(constant);
        return Constants.end() - Constants.begin();
    }
    return foundIter - Constants.begin();
}

IdT ConstantTable::FindInt(IntT i)
{
    const auto constant = Constant::CreateInt(i);
    const auto foundIter = std::find(Constants.begin(), Constants.end(), constant);
    if (foundIter == Constants.end())
    {
        Constants.push_back(constant);
        return Constants.end() - Constants.begin();
    }
    return foundIter - Constants.begin();
}

IdT ConstantTable::FindFloat(FloatT i)
{
    const auto constant = Constant::CreateFloat(i);
    const auto foundIter = std::find(Constants.begin(), Constants.end(), constant);
    if (foundIter == Constants.end())
    {
        Constants.push_back(constant);
        return Constants.end() - Constants.begin();
    }
    return foundIter - Constants.begin();
}

IdT ConstantTable::FindClass(std::string const& className)
{
    const auto constant = Constant::CreateClass(FindUtf8(className));
    const auto foundIter = std::find(Constants.begin(), Constants.end(), constant);
    if (foundIter == Constants.end())
    {
        Constants.push_back(constant);
        return Constants.end() - Constants.begin();
    }
    return foundIter - Constants.begin();
}

IdT ConstantTable::FindNaT(std::string const& name, std::string const& type)
{
    const auto constant = Constant::CreateNaT(FindUtf8(name), FindUtf8(type));
    const auto foundIter = std::find(Constants.begin(), Constants.end(), constant);
    if (foundIter == Constants.end())
    {
        Constants.push_back(constant);
        return Constants.end() - Constants.begin();
    }
    return foundIter - Constants.begin();
}

IdT ConstantTable::FindFieldRef(std::string const& className, std::string const& name, std::string const& type)
{
    const auto constant = Constant::CreateFieldRef(FindNaT(name, type), FindClass(className));
    const auto foundIter = std::find(Constants.begin(), Constants.end(), constant);
    if (foundIter == Constants.end())
    {
        Constants.push_back(constant);
        return Constants.end() - Constants.begin();
    }
    return foundIter - Constants.begin();
}

IdT ConstantTable::FindMethodRef(std::string const& className, std::string const& name, std::string const& type)
{
    const auto constant = Constant::CreateMethodRef(FindNaT(name, type), FindClass(className));
    const auto foundIter = std::find(Constants.begin(), Constants.end(), constant);
    if (foundIter == Constants.end())
    {
        Constants.push_back(constant);
        return Constants.end() - Constants.begin();
    }
    return foundIter - Constants.begin();
}

ClassAnalyzer::ClassAnalyzer(ClassDeclNode* node) : CurrentClass{ node }
{
}

void ClassAnalyzer::Analyze()
{
    Table.FindUtf8("Code");
    AnalyzeClass(CurrentClass);
}

void ClassAnalyzer::AnalyzeVarDecl(VarDeclNode* varDecl)
{
    if (!varDecl)
        return;
    varDecl->AType = varDecl->VarType->ToDataType();
    if (CurrentMethod) { CurrentMethod->variables.push_back(varDecl); }
    varDecl->InitExpr = AnalyzeExpr(varDecl->InitExpr);
}

void ClassAnalyzer::AnalyzeWhile(WhileNode* while_)
{
    if (!while_)
        return;
    while_->Condition = AnalyzeExpr(while_->Condition);
    AnalyzeStmt(while_->Body);
}

void ClassAnalyzer::AnalyzeDoWhile(DoWhileNode* doWhile)
{
    if (!doWhile)
        return;
    doWhile->Condition = AnalyzeExpr(doWhile->Condition);
    AnalyzeStmt(doWhile->Body);
}

void ClassAnalyzer::AnalyzeFor(ForNode* for_)
{
    if (!for_)
        return;
    AnalyzeVarDecl(for_->VarDecl);
    for_->FirstExpr = AnalyzeExpr(for_->FirstExpr);
    for_->Condition = AnalyzeExpr(for_->Condition);
    for_->IterExpr = AnalyzeExpr(for_->IterExpr);
    AnalyzeStmt(for_->Body);
}

void ClassAnalyzer::AnalyzeForEach(ForEachNode* forEach)
{
    if (!forEach)
        return;
    AnalyzeVarDecl(forEach->VarDecl);
    forEach->Expr = AnalyzeExpr(forEach->Expr);
    AnalyzeStmt(forEach->Body);
}

void ClassAnalyzer::AnalyzeIf(IfNode* if_)
{
    if (!if_)
        return;
    if_->Condition = AnalyzeExpr(if_->Condition);
    AnalyzeStmt(if_->ThenBranch);
    AnalyzeStmt(if_->ElseBranch);
}

void ClassAnalyzer::AnalyzeStmt(StmtNode* stmt)
{
    AnalyzeVarDecl(stmt->VarDecl);
    AnalyzeWhile(stmt->While);
    AnalyzeDoWhile(stmt->DoWhile);
    AnalyzeFor(stmt->For);
    AnalyzeForEach(stmt->ForEach);
    AnalyzeIf(stmt->If);
    stmt->Expr = AnalyzeExpr(stmt->Expr);
}

void ClassAnalyzer::AnalyzeMethod(MethodDeclNode* method)
{
    CurrentMethod = method;
    for (auto* stmt : method->Body->GetSeq()) { AnalyzeStmt(stmt); }
    CurrentMethod = nullptr;
}

void ClassAnalyzer::AnalyzeField(FieldDeclNode* field)
{
    AnalyzeVarDecl(field->VarDecl);
    const auto& allFields = CurrentClass->Members->Fields;
    const auto fieldNameCount = std::count_if(allFields.begin(), allFields.end(), [&](auto* other)
        {
            return field->VarDecl->Identifier == other->VarDecl->Identifier;
        });
    if (fieldNameCount > 1)
    {
        Errors.push_back("Field with name \"" + std::string{ field->VarDecl->Identifier } + "\" already defined!");
    }
}

void ClassAnalyzer::AnalyzeClass(ClassDeclNode* value)
{
    Table.FindClass(std::string{ value->ClassName });
    for (auto* field : value->Members->Fields) { AnalyzeField(field); }
    for (auto* method : value->Members->Methods) { AnalyzeMethod(method); }
}

[[nodiscard]] ExprNode* ClassAnalyzer::AnalyzeExpr(ExprNode* expr)
{
    if (!expr)
        return nullptr;
    auto* changed = ReplaceAssignmentsOnArrayElements(expr);
    CalculateTypesForExpr(changed);
    changed->ApplyToAllChildren(ReplaceAssignmentsOnArrayElements);
    return changed;
}

void ClassAnalyzer::CalculateTypesForExpr(ExprNode* node)
{
    if (!node)
        return;
    if (node->Type == ExprNode::TypeT::AccessExpr)
    {
        // ReSharper disable once CppIncompleteSwitchStatement
        // ReSharper disable once CppDefaultCaseNotHandledInSwitchStatement
        switch (node->Access->Type) // NOLINT(clang-diagnostic-switch)
        {
        case AccessExpr::TypeT::Integer:
            node->AType.AType = DataType::TypeT::Int;
            break;
        case AccessExpr::TypeT::Bool:
            node->AType.AType = DataType::TypeT::Bool;
            break;
        case AccessExpr::TypeT::String:
            node->AType.AType = DataType::TypeT::String;
            break;
        case AccessExpr::TypeT::Char:
            node->AType.AType = DataType::TypeT::Char;
            break;
        case AccessExpr::TypeT::Float:
            node->AType.AType = DataType::TypeT::Float;
            break;
        case AccessExpr::TypeT::Identifier:
        {
            auto isVariableFound = false;
            const auto name = std::string{ node->Access->Identifier };
            if (CurrentMethod)
            {
                if (auto* var = CurrentMethod->FindVariableByName(name); var)
                {
                    node->AType = var->AType;
                    isVariableFound = true;
                }
            }

            if (CurrentClass && !isVariableFound)
            {
                if (auto* var = CurrentClass->FindFieldByName(name); var)
                {
                    node->AType = var->VarDecl->AType;
                    isVariableFound = true;
                }
            }

            if (!isVariableFound) { Errors.push_back("Variable with name \"" + name + "\" is not found"); }
        }
        }
        return;
    }

    if (IsBinary(node->Type))
    {
        CalculateTypesForExpr(node->Left);
        CalculateTypesForExpr(node->Right);
        const auto leftType = node->Left->AType;
        const auto rightType = node->Right->AType;
        if (leftType == rightType)
        {
            node->AType = node->Left->AType;
            return;
        }
        Errors.push_back("Types '" + ToString(leftType) + "' and '" + ToString(rightType) + "' are not compatible");
    }

    if (IsUnary(node->Type))
    {
        CalculateTypesForExpr(node->Child);
        node->AType = node->Child->AType;
        return;
    }

    node->AType.IsUnknown = true;
}

ExprNode* ClassAnalyzer::ReplaceAssignmentsOnArrayElements(ExprNode* node)
{
    //if (node->Left != nullptr)
    //{
    //    node->Left = ReplaceAssignmentsOnArrayElements(node->Left);
    //    auto* converted = node->Left->ToAssignOnArrayElement();
    //    if (converted != nullptr)
    //        node->Left = converted;
    //}
    auto* converted = node->ToAssignOnArrayElement();
    if (converted)
        return converted;
    return node;
}
