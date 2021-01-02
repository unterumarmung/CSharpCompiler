// ReSharper disable CppCStyleCast
#include "ClassAnalyzer.h"
#include "Commands.h"
#include <iterator>
#include <algorithm>
#include <iostream>
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
        return lhs.ClassNameId == rhs.ClassNameId;
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

IdT ConstantTable::FindUtf8(std::string_view utf8)
{
    const auto constant = Constant::CreateUtf8(std::string{ utf8 });
    const auto foundIter = std::find(Constants.begin(), Constants.end(), constant);
    if (foundIter == Constants.end())
    {
        Constants.push_back(constant);
        return Constants.end() - Constants.begin();
    }
    return foundIter - Constants.begin() + 1;
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
    return foundIter - Constants.begin() + 1;
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
    return foundIter - Constants.begin() + 1;
}

IdT ConstantTable::FindClass(std::string_view className)
{
    const auto constant = Constant::CreateClass(FindUtf8(className));
    const auto foundIter = std::find(Constants.begin(), Constants.end(), constant);
    if (foundIter == Constants.end())
    {
        Constants.push_back(constant);
        return Constants.end() - Constants.begin();
    }
    return foundIter - Constants.begin() + 1;
}

IdT ConstantTable::FindNaT(std::string_view name, std::string_view type)
{
    const auto constant = Constant::CreateNaT(FindUtf8(name), FindUtf8(type));
    const auto foundIter = std::find(Constants.begin(), Constants.end(), constant);
    if (foundIter == Constants.end())
    {
        Constants.push_back(constant);
        return Constants.end() - Constants.begin();
    }
    return foundIter - Constants.begin() + 1;
}

IdT ConstantTable::FindFieldRef(std::string_view className, std::string_view name, std::string_view type)
{
    const auto constant = Constant::CreateFieldRef(FindNaT(name, type), FindClass(className));
    const auto foundIter = std::find(Constants.begin(), Constants.end(), constant);
    if (foundIter == Constants.end())
    {
        Constants.push_back(constant);
        return Constants.end() - Constants.begin();
    }
    return foundIter - Constants.begin() + 1;
}

IdT ConstantTable::FindMethodRef(std::string_view className, std::string_view name, std::string_view type)
{
    const auto constant = Constant::CreateMethodRef(FindNaT(name, type), FindClass(className));
    const auto foundIter = std::find(Constants.begin(), Constants.end(), constant);
    if (foundIter == Constants.end())
    {
        Constants.push_back(constant);
        return Constants.end() - Constants.begin();
    }
    return foundIter - Constants.begin() + 1;
}


ClassAnalyzer::ClassAnalyzer(ClassDeclNode* node, NamespaceDeclNode* namespace_, NamespaceDeclSeq* allNamespaces)
    : CurrentClass{ node }
  , Namespace{ namespace_ }
  , AllNamespaces{ allNamespaces }
{
    const auto hasConstructor = std::any_of(CurrentClass->Members->Methods.begin(),
                                            CurrentClass->Members->Methods.end(), [](auto* method)
                                            {
                                                return method->IsConstructor;
                                            });
    if (!hasConstructor)
        CurrentClass->Members->Add(new MethodDeclNode{
                                       VisibilityModifier::Public,
                                       nullptr,
                                       "<init>",
                                       MethodArguments::MakeEmpty(),
                                       StmtSeqNode::MakeEmpty()
                                   });
}

void ClassAnalyzer::Analyze() { AnalyzeClass(CurrentClass); }

void ClassAnalyzer::AnalyzeMemberSignatures()
{
    for (auto* method : CurrentClass->Members->Methods)
    {
        method->AReturnType = ToDataType(method->Type);
        ValidateTypename(method->AReturnType);
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

    if (CurrentMethod)
    {
        auto const& methodVariables = CurrentMethod->Variables;
        auto const foundVariable = std::find_if(methodVariables.begin(), methodVariables.end(), [&](VarDeclNode* var)
        {
            return var->Identifier == varDecl->Identifier;
        });
        if (foundVariable != methodVariables.end())
        {
            Errors.push_back("Variable with name '" + std::string{ varDecl->Identifier } +
                             "' is already defined in method " + std::string{ CurrentMethod->Identifier });
            return;
        }
        CurrentMethod->Variables.push_back(varDecl);
        varDecl->PositionInMethod = CurrentMethod->Variables.size() - 1;
    }
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

void ClassAnalyzer::AnalyzeReturn(StmtNode* node)
{
    if (node->Type != StmtNode::TypeT::Return)
        return;

    node->Expr = AnalyzeExpr(node->Expr);

    if (node->Expr == nullptr && CurrentMethod->AReturnType != DataType::VoidType)
    {
        Errors.emplace_back("Cannot return empty expression in non-void method " + std::string{
                                CurrentMethod->Identifier
                            });
    }
    if (node->Expr && node->Expr->AType != CurrentMethod->AReturnType)
    {
        Errors.emplace_back("Cannot return value of type " + ToString(node->Expr->AType) +
                            " in the method which return value is " + ToString(CurrentMethod->AReturnType));
    }
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
    AnalyzeReturn(stmt);
}


void ClassAnalyzer::AnalyzeMethod(MethodDeclNode* method)
{
    method->Class = CurrentClass;
    CurrentMethod = method;

    if (CurrentMethod->IsStatic)
    {
        const auto isMain = CurrentMethod->Identifier == "Main";
        const auto noArguments = CurrentMethod->ArgumentDtos.empty();
        if (!isMain || !noArguments)
        {
            Errors.emplace_back("Only Main method with no arguments can be static");
            return;
        }
        AllMains.push_back(method);
        CurrentMethod->Identifier = "main";
        // Локальная переменная args для мейна
        CurrentMethod->Variables.push_back(new VarDeclNode(nullptr, "", nullptr));
    }

    if (!CurrentMethod->IsStatic)
    {
        // Локальная переменная this
        auto thisVar = new VarDeclNode(nullptr, "this", nullptr);
        thisVar->AType = CurrentClass->ToDataType();
        CurrentMethod->Variables.push_back(thisVar);
    }

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
                         + " and with arguments of types: "
                         + ToString(ToTypes(method->ArgumentDtos))
                         + " has been already defined");
    }

    if (method->Body->GetSeq().empty() && method->AReturnType != DataType::VoidType)
    {
        Errors.push_back("There must be return statement in non-void method with name " + std::string{
                             method->Identifier
                         });
        CurrentMethod = nullptr;
        return;
    }

    for (auto* var : method->Arguments->GetSeq())
        AnalyzeVarDecl(var);

    for (auto* stmt : method->Body->GetSeq()) { AnalyzeStmt(stmt); }

    if (!method->Body->GetSeq().empty()
        && method->Body->GetSeq().back()->Type != StmtNode::TypeT::Return
        && method->AReturnType != DataType::VoidType)
    {
        Errors.push_back("Last statement in method " + std::string{ method->Identifier } + " must be return!");
    }

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
    value->Namespace = this->Namespace;

    for (auto* field : value->Members->Fields) { AnalyzeField(field); }
    for (auto* method : value->Members->Methods)
    {
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
               ToTypes(method->ArgumentDtos) && !method->IsStatic;
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
               ToTypes(method->ArgumentDtos) && !method->IsStatic;
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

    if (node->Type == ExprNode::TypeT::Cast)
    {
        const auto castType = ToDataType(node->StandardTypeChild);
        CalculateTypesForExpr(node->Child);
        const auto& exprType = node->Child->AType;

        const auto anyIsBool = castType == DataType::BoolType
                               || exprType == DataType::BoolType;
        const auto anyIsVoid = castType == DataType::VoidType
                               || exprType == DataType::VoidType;
        const auto anyIsUnknown = castType.IsUnknown || exprType.IsUnknown;
        if (anyIsBool || anyIsVoid || anyIsUnknown)
        {
            Errors.push_back("Cannot cast '" + ToString(exprType) + "' to '" + ToString(castType) + "'");
            node->AType.IsUnknown = true;
            return;
        }
    }

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
                access->ActualVar = var;
            }
        }

        if (CurrentClass && !isVariableFound)
        {
            if (auto* var = CurrentClass->FindFieldByName(name); var)
            {
                type = var->VarDecl->AType;
                isVariableFound = true;
                access->ActualField = var;
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
        NamespaceDeclNode* namespace_ = Namespace;
        if (dataType.ComplexType.size() > 1)
        {
            const auto foundNamespace = std::find_if(AllNamespaces->GetSeq().begin(), AllNamespaces->GetSeq().end(),
                                                     [&](NamespaceDeclNode* namespaceDecl)
                                                     {
                                                         return namespaceDecl->NamespaceName == dataType.ComplexType.
                                                                front();
                                                     });
            if (foundNamespace == AllNamespaces->GetSeq().end())
            {
                Errors.push_back("No namespace called " + std::string{ dataType.ComplexType.front() });
                return;
            }
            namespace_ = *foundNamespace;
        }
        const auto& allClassesInNamespace = namespace_->Members->Classes;
        const auto foundClass = std::find_if(allClassesInNamespace.begin(), allClassesInNamespace.end(),
                                             [&](ClassDeclNode* class_)
                                             {
                                                 return class_->ClassName == dataType.ComplexType.back();
                                             });
        if (foundClass == allClassesInNamespace.end())
        {
            Errors.push_back("No class " + dataType.ComplexType.back() + " in namespace " + std::string{
                                 namespace_->NamespaceName
                             });
            dataType.IsUnknown = true;
        }
        else if (dataType.ComplexType.size() == 1)
        {
            dataType.ComplexType.insert(dataType.ComplexType.begin(), std::string{ Namespace->NamespaceName });
        }
    }
}

ClassDeclNode* ClassAnalyzer::FindClass(DataType const& dataType) const
{
    if (dataType.AType != DataType::TypeT::Complex)
        return nullptr;
    if (dataType.ArrayArity > 0)
        return nullptr;
    if (dataType.ComplexType.size() == 1)
    {
        for (auto* class_ : Namespace->Members->Classes)
        {
            if (dataType.ComplexType.back() == class_->ClassName)
                return class_;
        }
    }
    else
    {
        const auto& allNamespaces = AllNamespaces->GetSeq();
        const auto foundNamespace = std::find_if(allNamespaces.begin(), allNamespaces.end(),
                                                 [&](NamespaceDeclNode const* const namespace_)
                                                 {
                                                     return dataType.ComplexType.front() == namespace_->NamespaceName;
                                                 });
        if (foundNamespace == allNamespaces.end())
            return nullptr;
        for (auto* class_ : (*foundNamespace)->Members->Classes)
        {
            if (dataType.ComplexType.back() == class_->ClassName)
                return class_;
        }
    }

    return nullptr;
}

void ClassAnalyzer::FillTables(FieldDeclNode* field)
{
    const auto nameId = File.Constants.FindUtf8(field->VarDecl->Identifier);
    const auto typeId = File.Constants.FindUtf8(field->VarDecl->AType.ToDescriptor());
    const auto accessFlags = ToAccessFlags(field->Visibility);
    File.Fields.push_back({ nameId, typeId, accessFlags });
}

void ClassAnalyzer::FillTables(MethodDeclNode* method)
{
    const auto nameId = File.Constants.FindUtf8(method->Identifier);
    const auto methodDescriptor = method->Identifier == "main" && method->IsStatic
        ?  "([Ljava/lang/String;)V"
        : method->ToDescriptor();

    const auto typeId = File.Constants.FindUtf8(methodDescriptor);
    const auto accessFlags = ToAccessFlags(method->Visibility, method->IsStatic);
    File.Methods.push_back({ nameId, typeId, accessFlags, method });
}

void ClassAnalyzer::FillTables()
{
    for (auto* field : CurrentClass->Members->Fields) { FillTables(field); }
    for (auto* method : CurrentClass->Members->Methods) { FillTables(method); }
}

Bytes ToBytes(const ConstantTable& constants)
{
    auto bytes = ToBytes((uint16_t)(constants.Constants.size() + 1));
    for (auto const& constant : constants.Constants) { append(bytes, ToBytes(constant)); }
    return bytes;
}

Bytes ToBytes(JvmField field)
{
    Bytes bytes;
    append(bytes, ToBytes(static_cast<uint16_t>(field.AccessFlags)));
    append(bytes, ToBytes(field.NameId));
    append(bytes, ToBytes(field.TypeId));
    constexpr auto attributesCount = (uint16_t)0;
    append(bytes, ToBytes(attributesCount));
    return bytes;
}

Bytes ToBytes(ExprNode* expr, ClassFile& file);

Bytes ToBytes(AccessExpr* expr, ClassFile& file)
{
    switch (expr->Type) {
    case AccessExpr::TypeT::Expr:
        return ToBytes(expr->Child, file);
    case AccessExpr::TypeT::ArrayElementExpr:
        break;
    case AccessExpr::TypeT::ComplexArrayType:
        break;
    case AccessExpr::TypeT::Integer:
    {
        Bytes bytes;
        const auto intVal = expr->Integer;
        if (intVal >= -32768 && intVal <= 32767)
        {
            const auto intBytes = ToBytes((IntT)intVal);
            bytes.push_back((uint8_t)Command::sipush);
            bytes.push_back(intBytes[2]);
            bytes.push_back(intBytes[3]);
        }
        else
        {
            const auto constantId = file.Constants.FindInt(expr->Integer);
            const auto constantIdBytes = ToBytes(constantId);
            append(bytes, (uint8_t)Command::ldc_w);
            append(bytes, constantIdBytes);
        }
        return bytes;
    }
    case AccessExpr::TypeT::Float:
        break;
    case AccessExpr::TypeT::String:
        break;
    case AccessExpr::TypeT::Char:
        break;
    case AccessExpr::TypeT::Bool:
        break;
    case AccessExpr::TypeT::Identifier:
        break;
    case AccessExpr::TypeT::SimpleMethodCall:
        break;
    case AccessExpr::TypeT::Dot:
        break;
    case AccessExpr::TypeT::DotMethodCall:
        break;
    default: ;
    }
    return {};
}

Bytes ToBytes(ExprNode* expr, ClassFile& file)
{
    if (IsBinary(expr->Type))
    {
        Bytes bytes;
        const auto leftBytes = ToBytes(expr->Left, file);
        const auto rightBytes = ToBytes(expr->Right, file);
        append(bytes, leftBytes);
        append(bytes, rightBytes);
        if (expr->AType != DataType::IntType)
        {
            throw std::runtime_error{ "Only ints are supported" };
        }
        switch (expr->Type)  // NOLINT(clang-diagnostic-switch-enum)
        {
        case ExprNode::TypeT::BinPlus:
            append(bytes, (uint8_t)Command::iadd);
            break;
        case ExprNode::TypeT::BinMinus:
            append(bytes, (uint8_t)Command::isub);
            break;
        case ExprNode::TypeT::Multiply:
            append(bytes, (uint8_t)Command::imul);
            break;
        case ExprNode::TypeT::Divide:
            append(bytes, (uint8_t)Command::idiv);
            break;
        default: throw std::runtime_error{ "Not supported" };
        }
        return bytes;
    }

    if (expr->Type == ExprNode::TypeT::AccessExpr)
    {
        return ToBytes(expr->Access, file);
    }

    if (expr->Type == ExprNode::TypeT::SimpleNew)
    {
        const auto type = expr->AType;
        if (type.AType != DataType::TypeT::Complex)
            throw std::runtime_error{ "Cannot create object of type " + ToString(type) };
        const auto classIdConstant = file.Constants.FindClass(type.ToClassName());

        Bytes bytes;
        append(bytes, (uint8_t)Command::new_);
        append(bytes, ToBytes(classIdConstant));
        append(bytes, (uint8_t)Command::dup);

        const auto constructorRef = file.Constants.FindMethodRef(type.ToClassName(), "<init>", "()V");
        append(bytes, (uint8_t)Command::invokespecial);
        append(bytes, ToBytes(constructorRef));

        return bytes;
    }


    return {};
}

Bytes ToBytes(VarDeclNode* node, ClassFile& file)
{
    Bytes bytes;

    // Инициализация переменной
    if (node->InitExpr)
    {
        append(bytes, ToBytes(node->InitExpr, file));
    }
    else
    {
        if (node->AType == DataType::IntType)
        {
            append(bytes, (uint8_t)Command::iconst_0);
        }
        else if (node->AType.AType == DataType::TypeT::Complex)
        {
            append(bytes, (uint8_t)Command::aconst_null);
        }
        else
        {
            throw std::runtime_error("unsupported type of variable " + ToString(node->AType));
        }
    }

    if (node->AType == DataType::IntType)
    {
        append(bytes, (uint8_t)Command::istore);
        
    }
    else if (node->AType.AType == DataType::TypeT::Complex)
    {
        append(bytes, (uint8_t)Command::astore);
    }

    append(bytes, (uint8_t)node->PositionInMethod);

    return bytes;
}

Bytes ToBytes(StmtNode* stmt, ClassFile& file)
{
    Bytes bytes;

    switch (stmt->Type) {
    case StmtNode::TypeT::Empty:
        return bytes;
    case StmtNode::TypeT::VarDecl:
        return ToBytes(stmt->VarDecl, file);
    case StmtNode::TypeT::While:
        break;
    case StmtNode::TypeT::DoWhile:
        break;
    case StmtNode::TypeT::For:
        break;
    case StmtNode::TypeT::Foreach:
        break;
    case StmtNode::TypeT::BlockStmt:
        break;
    case StmtNode::TypeT::IfStmt:
        break;
    case StmtNode::TypeT::Return:
        break;
    case StmtNode::TypeT::ExprStmt:
        return ToBytes(stmt->Expr, file);
    default: ;
    }
    return {};
}



Bytes ToBytes(MethodDeclNode* method, ClassFile& classFile)
{
    Bytes bytes;

    constexpr auto stackSize = (uint16_t)1000;
    append(bytes, ToBytes(stackSize));

    const uint16_t localVariablesCount = method->Variables.size();

    append(bytes, ToBytes(localVariablesCount));

    Bytes codeBytes;

    if (method->IsConstructor)
    {
        append(codeBytes, (uint8_t)Command::aload_0);
        append(codeBytes, (uint8_t)Command::invokespecial);
        const auto javaBaseObjectConstructor = classFile.Constants.FindMethodRef(
             JAVA_BASE_OBJECT.ToClassName(),
             "<init>",
             "()V"
            );
        append(codeBytes, ToBytes(javaBaseObjectConstructor));
    }

    for (auto* stmt : method->Body->GetSeq())
    {
        append(codeBytes, ToBytes(stmt, classFile));
    }

    append(codeBytes, (uint8_t)Command::return_);

    append(bytes, ToBytes((uint32_t)codeBytes.size()));
    append(bytes, codeBytes);

    constexpr auto exceptionTableSize = (uint16_t)0;
    constexpr auto attributesTableSize = (uint16_t)0;

    append(bytes, ToBytes(exceptionTableSize));
    append(bytes, ToBytes(attributesTableSize));

    return bytes;
}

Bytes ToBytes(JvmMethod method, ClassFile& classFile)
{
    Bytes bytes;
    append(bytes, ToBytes(static_cast<uint16_t>(method.AccessFlags)));
    append(bytes, ToBytes(method.NameId));
    append(bytes, ToBytes(method.TypeId));
    constexpr auto attributesCount = (uint16_t)1; // The only attribute is Code
    append(bytes, ToBytes(attributesCount));
    append(bytes, ToBytes(classFile.Constants.FindUtf8("Code")));
    const auto codeBytes = ToBytes(method.ActualMethod, classFile);
    auto codeBytesLength = ToBytes((uint32_t)codeBytes.size());
    append(bytes, codeBytesLength);
    append(bytes, codeBytes);
    return bytes;
}

#include <filesystem>
#include <fstream>

void ClassAnalyzer::Generate()
{
    using namespace std::filesystem;
    const auto filename = std::string{ CurrentClass->ClassName } + ".class";
    auto filepath = current_path() / "Output" / Namespace->NamespaceName / filename;
    create_directory(current_path() / "Output" / Namespace->NamespaceName);
    std::fstream out{ filepath, std::ios_base::out | std::ios_base::binary | std::ios_base::trunc };
    out << (char)0xCA << (char)0xFE << (char)0xBA << (char)0xBE;
    const auto minorVersion = ::ToBytes(ClassFile::MinorVersion);
    out.write((char*)minorVersion.data(), minorVersion.size());
    const auto majorVersion = ::ToBytes(ClassFile::MajorVersion);
    out.write((char*)majorVersion.data(), majorVersion.size());
    auto const classBytes = this->ToBytes();
    auto const constantBytes = ::ToBytes(File.Constants);
    out.write((char*)constantBytes.data(), constantBytes.size());

    auto position = out.tellp();

    out.write((char*)classBytes.data(), classBytes.size());

    const auto classAttributesCount = ::ToBytes((uint16_t)0);
    out.write((char*)classAttributesCount.data(), classAttributesCount.size());
}

Bytes ClassAnalyzer::ToBytes()
{
    Bytes bytes;
    const auto classConstantId = File.Constants.FindClass(CurrentClass->ToDataType().ToClassName());
    const auto superClassId = File.Constants.FindClass(JAVA_BASE_OBJECT.ToClassName());
    const auto accessFlags = AccessFlags::Super | AccessFlags::Public;
    append(bytes, ::ToBytes((uint16_t)accessFlags));
    append(bytes, ::ToBytes(classConstantId));
    append(bytes, ::ToBytes(superClassId));

    constexpr auto interfacesCount = (uint16_t)0;
    append(bytes, ::ToBytes(interfacesCount));

    append(bytes, ::ToBytes((uint16_t)File.Fields.size()));
    for (auto field : File.Fields) { append(bytes, ::ToBytes(field)); }

    std::sort(File.Methods.begin(), File.Methods.end(), [](auto const& lhs, auto const& rhs)
        {
            return lhs.ActualMethod->IsConstructor > rhs.ActualMethod->IsConstructor;
        });
    append(bytes, ::ToBytes((uint16_t)File.Methods.size()));
    for (auto method : File.Methods) { append(bytes, ::ToBytes(method, File)); }
    return bytes;
}

// #include <WinSock2.h>
// #pragma comment(lib, "Ws2_32.lib")
Bytes ToBytes(const uint32_t n)
{
    const auto netNum = n;

    union
    {
        uint32_t num;
        unsigned char bytes[sizeof uint32_t];
    } u;

    u.num = netNum;

    Bytes bytes(sizeof n, '\0');

    std::copy(std::rbegin(u.bytes), std::rend(u.bytes), bytes.begin());

    return bytes;
}

Bytes ToBytes(const uint16_t n)
{
    const auto netNum = n;
    Bytes bytes(sizeof n, '\0');
    for (int i = 0; i < bytes.size(); i++)
        bytes[bytes.size() - 1 - i] = (netNum >> (i * 8));
    return bytes;
}

Bytes ToBytes(const IntT n)
{
    const auto netNum = n;
    Bytes bytes(sizeof n, '\0');
    for (int i = 0; i < bytes.size(); i++)
        bytes[bytes.size() - 1 - i] = (netNum >> (i * 8));
    return bytes;
}

Bytes ToBytes(Constant const& constant)
{
    Bytes bytes;
    append(bytes, static_cast<uint8_t>(constant.Type));
    switch (constant.Type)
    {
    case Constant::TypeT::Utf8:
        append(bytes, ToBytes((uint16_t)constant.Utf8.size()));
        append(bytes, constant.Utf8);
        break;
    case Constant::TypeT::Integer:
        append(bytes, ToBytes(constant.Integer));
        break;
    case Constant::TypeT::Float:
        throw std::runtime_error{ "Float is not supported" };
    case Constant::TypeT::String:
        append(bytes, ToBytes(constant.Utf8Id));
        break;
    case Constant::TypeT::NameAndType:
        append(bytes, ToBytes(constant.NameId));
        append(bytes, ToBytes(constant.TypeId));
        break;
    case Constant::TypeT::Class:
        append(bytes, ToBytes(constant.ClassNameId));
        break;
    case Constant::TypeT::MethodRef:
        append(bytes, ToBytes(constant.ClassId));
        append(bytes, ToBytes(constant.NameAndTypeId));
        break;
    case Constant::TypeT::FieldRef:
        append(bytes, ToBytes(constant.ClassId));
        append(bytes, ToBytes(constant.NameAndTypeId));
        break;
    default: ;
    }
    return bytes;
}
