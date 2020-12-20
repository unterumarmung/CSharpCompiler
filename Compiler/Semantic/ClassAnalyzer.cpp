#include "ClassAnalyzer.h"

#include <iterator>

using namespace std::string_literals;

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

ClassAnalyzer::ClassAnalyzer(ClassDeclNode* node, NamespaceDeclNode* namespace_) : CurrentClass{ node }
  , Namespace{ namespace_ }
{
}

void ClassAnalyzer::Analyze()
{
    Table.FindUtf8("Code");
    AnalyzeClass(CurrentClass);
}

void ClassAnalyzer::AnalyzeMemberSignatures()
{
    for (auto* method : CurrentClass->Members->Methods)
    {
        method->AReturnType = ToDataType(method->Type);
        for (auto* var : method->Arguments->GetSeq())
            AnalyzeVarDecl(var);
    }
    for (auto* field : CurrentClass->Members->Fields) { AnalyzeVarDecl(field->VarDecl, false); }
}

void ClassAnalyzer::AnalyzeVarDecl(VarDeclNode* varDecl, bool withInit)
{
    if (!varDecl)
        return;
    varDecl->AType = ToDataType(varDecl->VarType);
    ValidateTypename(varDecl->AType);
    varDecl->InitExpr = AnalyzeExpr(varDecl->InitExpr);

    if (withInit && varDecl->InitExpr && varDecl->AType != varDecl->InitExpr->AType)
    {
        Errors.push_back("Cannot initialize variable of type " + ToString(varDecl->AType) + " with object of type " +
                         ToString(varDecl->InitExpr->AType));
    }

    if (CurrentMethod) { CurrentMethod->Variables.push_back(varDecl); }
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
    method->Class = CurrentClass;
    CurrentMethod = method;

    const auto& allMethods = CurrentClass->Members->Methods;
    const auto sameMethodsCount = std::count_if(allMethods.begin(), allMethods.end(), [&](auto* otherMethod)
    {
        return method->Identifier == otherMethod->Identifier &&
               ToTypes(method->ArgumentDtos) ==
               ToTypes(otherMethod->ArgumentDtos);
    });

    if (sameMethodsCount > 1)
    {
        Errors.push_back("Method with name "
                         + std::string(method->Identifier)
                         + "and with arguments of types: "
                         + ToString(ToTypes(method->ArgumentDtos))
                         + " has been already defined");
    }

    for (auto* var : method->Arguments->GetSeq())
        AnalyzeVarDecl(var);
    for (auto* stmt : method->Body->GetSeq()) { AnalyzeStmt(stmt); }
    CurrentMethod = nullptr;
}

void ClassAnalyzer::AnalyzeField(FieldDeclNode* field)
{
    field->Class = CurrentClass;
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
    for (auto* method : value->Members->Methods)
    {
        method->AReturnType = ToDataType(method->Type);
        auto&& varDeclNodes = method->Arguments->GetSeq();
        std::transform(varDeclNodes.begin(), varDeclNodes.end(),
                       std::back_inserter(method->ArgumentDtos),
                       ToMethodArgumentDto);
    }
    for (auto* method : value->Members->Methods) { AnalyzeMethod(method); }
}

[[nodiscard]] ExprNode* ClassAnalyzer::AnalyzeExpr(ExprNode* expr)
{
    if (!expr)
        return nullptr;
    auto* changed = ReplaceAssignmentsOnArrayElements(expr);
    changed->ApplyToAllChildren(ReplaceAssignmentsOnArrayElements);
    AnalyzeAccessExpr(expr->Access);
    AnalyzeAccessExpr(expr->ArrayExpr);
    expr->CallForAllChildren([this](ExprNode* expr)
    {
        AnalyzeAccessExpr(expr->Access);
        AnalyzeAccessExpr(expr->ArrayExpr);
    });
    CalculateTypesForExpr(changed);
    return changed;
}

void ClassAnalyzer::AnalyzeAccessExpr(AccessExpr* expr)
{
    if (!expr)
        return;
    AnalyzeSimpleMethodCall(expr);
    AnalyzeDotMethodCall(expr);
}

void ClassAnalyzer::AnalyzeSimpleMethodCall(ExprNode* node)
{
    if (!node)
        return;
    auto* expr = node->Access;
    AnalyzeSimpleMethodCall(expr);
}

void ClassAnalyzer::AnalyzeDotMethodCall(ExprNode* node)
{
    if (!node)
        return;
    auto* expr = node->Access;
    AnalyzeDotMethodCall(expr);
}

void ClassAnalyzer::AnalyzeSimpleMethodCall(AccessExpr* expr)
{
    if (!expr)
        return;
    if (expr->Type != AccessExpr::TypeT::SimpleMethodCall)
        return;

    for (auto*& argument : expr->Arguments->GetSeq())
        argument = AnalyzeExpr(argument);

    const auto methodName = expr->Identifier;
    const auto callTypes = [expr]()
    {
        auto const& arguments = expr->Arguments->GetSeq();
        std::vector<DataType> types(arguments.size());
        std::transform(arguments.begin(), arguments.end(), types.begin(), [](auto& arg) { return arg->AType; });
        return types;
    }();

    auto const& allMethods = CurrentClass->Members->Methods;
    const auto foundMethod = std::find_if(allMethods.begin(), allMethods.end(), [&](auto* method)
    {
        return methodName == method->Identifier && callTypes ==
               ToTypes(method->ArgumentDtos);
    });

    if (foundMethod == allMethods.end())
    {
        Errors.push_back("Cannot call method with name " + std::string{ methodName } + " with arguments of types " +
                         ToString(callTypes));
        return;
    }

    expr->ActualMethodCall = *foundMethod;
}

void ClassAnalyzer::AnalyzeDotMethodCall(AccessExpr* expr)
{
    if (!expr)
        return;
    if (expr->Type != AccessExpr::TypeT::DotMethodCall)
        return;

    for (auto*& argument : expr->Arguments->GetSeq())
        argument = AnalyzeExpr(argument);

    AnalyzeAccessExpr(expr->Previous);
    const auto typeForPrevious = CalculateTypeForAccessExpr(expr->Previous);
    auto* foundClass = FindClass(typeForPrevious);
    if (foundClass == nullptr)
    {
        Errors.push_back("No member " + std::string{ expr->Identifier } + " in type " + ToString(typeForPrevious));
        return;
    }

    const auto methodName = expr->Identifier;
    const auto callTypes = [expr]()
    {
        auto const& arguments = expr->Arguments->GetSeq();
        std::vector<DataType> types(arguments.size());
        std::transform(arguments.begin(), arguments.end(), types.begin(), [](auto& arg) { return arg->AType; });
        return types;
    }();

    auto const& allMethods = foundClass->Members->Methods;
    const auto foundMethod = std::find_if(allMethods.begin(), allMethods.end(), [&](auto* method)
    {
        return methodName == method->Identifier && callTypes ==
               ToTypes(method->ArgumentDtos);
    });

    if (foundMethod == allMethods.end())
    {
        Errors.push_back("Cannot call method with name " + std::string{ methodName } + " with arguments of types " +
                         ToString(callTypes));
        return;
    }

    expr->ActualMethodCall = *foundMethod;
}

auto IsIndexType(const DataType& data) -> bool { return data.AType == DataType::TypeT::Int && data.ArrayArity == 0; }

void ClassAnalyzer::CalculateTypesForExpr(ExprNode* node)
{
    if (!node)
        return;
    if (node->Type == ExprNode::TypeT::AccessExpr)
    {
        node->AType = CalculateTypeForAccessExpr(node->Access);
        return;
    }

    if (node->Type == ExprNode::TypeT::SimpleNew)
    {
        auto dataType = ToDataType(node->TypeNode);
        ValidateTypename(dataType);
        node->AType = dataType;
        return;
    }

    if (node->Type == ExprNode::TypeT::ArrayNew)
    {
        if (const auto& elements = node->ExprSeq->GetSeq();
            !elements.empty())
        {
            for (auto* element : elements) { CalculateTypesForExpr(element); }
            auto type = elements.front()->AType;
            std::vector<DataType> types(elements.size());
            std::transform(elements.begin(), elements.end(), types.begin(), [](ExprNode* node) { return node->AType; });
            const bool allElementsHaveSameType = std::all_of(types.begin(), types.end(),
                                                             [&type](auto& other) { return type == other; });

            if (!allElementsHaveSameType)
            {
                Errors.push_back("Cannot create array with different types: " + ToString(types));
                return;
            }
        }

        if (node->TypeNode != nullptr)
        {
            const auto dataType = ToDataType(node->TypeNode);
            if (dataType.ArrayArity == 0)
            {
                Errors.push_back("Cannot initialize array with array-like syntax the type " + ToString(dataType));
                return;
            }
        }
    }

    if (node->Type == ExprNode::TypeT::StandardArrayNew)
    {
        CalculateTypesForExpr(node->Child);
        auto dataType = ToDataType(node->StandardTypeChild);
        dataType.ArrayArity += 1;
        node->AType = dataType;
        if (node->Child->AType != DataType{ DataType::TypeT::Int })
        {
            Errors.push_back("Array size must be int, not " + ToString(node->Child->AType));
        }
        return;
    }

    const DataType boolType = { DataType::TypeT::Bool, {}, {}, {} };
    if (IsBinary(node->Type))
    {
        CalculateTypesForExpr(node->Left);
        CalculateTypesForExpr(node->Right);
        const auto& leftType = node->Left->AType;
        const auto& rightType = node->Right->AType;
        if (leftType == rightType)
        {
            node->AType = leftType;
            return;
        }
        if (IsLogical(node->Type))
        {
            node->AType = boolType;
            if (boolType == leftType && boolType == rightType)
                return;
        }
        Errors.push_back("Types '" + ToString(leftType) + "' and '" + ToString(rightType) +
                         "' are not compatible with operation " + ToString(node->Type));
    }

    if (IsUnary(node->Type))
    {
        CalculateTypesForExpr(node->Child);
        node->AType = node->Child->AType;

        if (IsLogical(node->Type) && node->Child->AType != boolType)
        {
            node->AType = boolType;
            Errors.push_back("Type '" + ToString(node->Child->AType) + "' is not compatible with operation " +
                             ToString(node->Type));
        }
        return;
    }

    if (node->Type == ExprNode::TypeT::AssignOnArrayElement)
    {
        const auto dataTypeForArray = CalculateTypeForAccessExpr(node->ArrayExpr);
        CalculateTypesForExpr(node->IndexExpr);
        const auto indexType = node->IndexExpr->AType;
        CalculateTypesForExpr(node->AssignExpr);
        if (!IsIndexType(indexType))
        {
            Errors.push_back("Array index must be type int, not " + ToString(indexType));
            return;
        }

        auto thisType = dataTypeForArray;
        thisType.ArrayArity -= 1;

        if (thisType != node->AssignExpr->AType)
        {
            Errors.push_back("Cannot assign value of type " + ToString(node->AssignExpr->AType) + " to value of type " +
                             ToString(thisType));
            return;
        }

        node->AType = thisType;
        return;
    }

    node->AType.IsUnknown = true;
}

DataType ClassAnalyzer::CalculateTypeForAccessExpr(AccessExpr* access)
{
    DataType type;
    // ReSharper disable once CppIncompleteSwitchStatement
    // ReSharper disable once CppDefaultCaseNotHandledInSwitchStatement
    switch (access->Type) // NOLINT(clang-diagnostic-switch)
    {
    case AccessExpr::TypeT::Integer:
        type.AType = DataType::TypeT::Int;
        access->AType = type;
        return type;
    case AccessExpr::TypeT::Bool:
        type.AType = DataType::TypeT::Bool;
        access->AType = type;
        return type;
    case AccessExpr::TypeT::String:
        type.AType = DataType::TypeT::String;
        access->AType = type;
        return type;
    case AccessExpr::TypeT::Char:
        type.AType = DataType::TypeT::Char;
        access->AType = type;
        return type;
    case AccessExpr::TypeT::Float:
        type.AType = DataType::TypeT::Float;
        access->AType = type;
        return type;
    case AccessExpr::TypeT::SimpleMethodCall:
        if (access->ActualMethodCall)
        {
            type = access->ActualMethodCall->AReturnType;
            access->AType = type;
        }
        else
        {
            type = { DataType::TypeT::Void, {}, true };
            access->AType = type;
        }
        return type;
    case AccessExpr::TypeT::ArrayElementExpr:
    {
        const auto dataTypeForPrevious = CalculateTypeForAccessExpr(access->Previous);
        CalculateTypesForExpr(access->Child);
        const auto childType = access->Child->AType;
        if (!IsIndexType(childType))
        {
            Errors.push_back("Array index must be type int, not " + ToString(childType));
            access->AType = type;
            return type;
        }
        if (dataTypeForPrevious.ArrayArity == 0)
        {
            Errors.push_back("Cannot use operator[] on type " + ToString(dataTypeForPrevious));
            access->AType = type;
            return type;
        }
        auto thisDataType = dataTypeForPrevious;
        thisDataType.ArrayArity -= 1;
        access->AType = thisDataType;
        return thisDataType;
    }
    case AccessExpr::TypeT::Dot:
    {
        auto typeForPrevious = CalculateTypeForAccessExpr(access->Previous);
        auto* foundClass = FindClass(typeForPrevious);
        if (foundClass == nullptr)
        {
            Errors.push_back("No member " + std::string{
                                 access->Identifier
                             } + " in type " + ToString(typeForPrevious));
            type.IsUnknown = true;
            access->AType = type;
            return type;
        }
        auto const& fields = foundClass->Members->Fields;
        const auto foundField = std::find_if(fields.begin(), fields.end(), [&](FieldDeclNode* field)
        {
            return field->VarDecl->Identifier == access->Identifier;
        });
        if (foundField == fields.end())
        {
            Errors.push_back("No member " + std::string{
                                 access->Identifier
                             } + " in type " + ToString(typeForPrevious));
            type.IsUnknown = true;
            access->AType = type;
            return type;
        }
        access->ActualField = *foundField;
        access->AType = access->ActualField->VarDecl->AType;
        return access->AType;
    }
    case AccessExpr::TypeT::DotMethodCall:
    {
        if (access->ActualMethodCall)
        {
            type = access->ActualMethodCall->AReturnType;
            access->AType = type;
        }
        else
        {
            type = { DataType::TypeT::Void, {}, true };
            access->AType = type;
        }
        return type;
    }
    case AccessExpr::TypeT::Identifier:
    {
        auto isVariableFound = false;
        const auto name = std::string{ access->Identifier };
        if (CurrentMethod)
        {
            if (auto* var = CurrentMethod->FindVariableByName(name); var)
            {
                type = var->AType;
                isVariableFound = true;
            }
        }

        if (CurrentClass && !isVariableFound)
        {
            if (auto* var = CurrentClass->FindFieldByName(name); var)
            {
                type = var->VarDecl->AType;
                isVariableFound = true;
            }
        }

        if (isVariableFound)
        {
            access->AType = type;
            return type;
        }

        Errors.push_back("Variable with name \"" + name + "\" is not found");
    }
    }
    type.IsUnknown = true;
    access->AType = type;
    return type;
}

ExprNode* ClassAnalyzer::ReplaceAssignmentsOnArrayElements(ExprNode* node)
{
    auto* converted = node->ToAssignOnArrayElement();
    if (converted)
        return converted;
    return node;
}

void ClassAnalyzer::ValidateTypename(DataType& dataType)
{
    if (dataType.IsUnknown) { Errors.emplace_back("Cannot create object of unknown type"); }
    else if (dataType.AType == DataType::TypeT::Complex)
    {
        const auto& allClassesInNamespace = Namespace->Members->Classes;
        const auto foundClass = std::find_if(allClassesInNamespace.begin(), allClassesInNamespace.end(),
                                             [&](ClassDeclNode* class_)
                                             {
                                                 return class_->ClassName == dataType.ComplexType.front();
                                             });
        if (foundClass == allClassesInNamespace.end())
        {
            Errors.push_back("No class " + ToString(dataType) + "in namespace " + std::string{
                                 Namespace->NamespaceName
                             });
            dataType.IsUnknown = true;
        }
        else { dataType.ComplexType.insert(dataType.ComplexType.begin(), std::string{ Namespace->NamespaceName }); }
    }
}

ClassDeclNode* ClassAnalyzer::FindClass(DataType const& dataType) const
{
    if (dataType.AType != DataType::TypeT::Complex)
        return nullptr;
    if (dataType.ArrayArity > 0)
        return nullptr;
    for (auto* class_ : Namespace->Members->Classes)
    {
        if (dataType.ComplexType.back() == class_->ClassName)
            return class_;
    }
    return nullptr;
}
