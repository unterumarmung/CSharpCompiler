#pragma once
#include <string>
#include <cstdint>
#include <vector>
#include <algorithm>
#include "../Tree/Program.h"

using IdT = unsigned long long;
using IntT = std::int32_t;
using FloatT = double;

struct Constant
{
    enum class TypeT
    {
        Utf8,
        Integer,
        Float,
        String,
        NameAndType,
        Class,
        MethodRef,
        FieldRef
    } Type{};

    std::string Utf8{};
    IntT Integer{};
    FloatT Float{};

    IdT Utf8Id{};

    IdT NameId{};
    IdT TypeId{};

    IdT ClassNameId{};

    IdT NameAndTypeId{};
    IdT ClassId{};


    friend bool operator==(const Constant& lhs, const Constant& rhs)
    {
        if  (lhs.Type != rhs.Type)
        {
            return false;
        }
        switch (lhs.Type) {
        case TypeT::Utf8:
            return lhs.Utf8 == rhs.Utf8;
        case TypeT::Integer:
            return lhs.Integer == rhs.Integer;
        case TypeT::Float:
            return lhs.Float == rhs.Float;
        case TypeT::String:
        case TypeT::Class:
            return lhs.Utf8Id == rhs.Utf8Id;
        case TypeT::NameAndType:
            return lhs.NameId == rhs.NameId && lhs.TypeId == rhs.TypeId;
        case TypeT::MethodRef:
        case TypeT::FieldRef:
            return lhs.NameAndTypeId == rhs.NameAndTypeId && lhs.ClassId == rhs.ClassId;
        }
    }
    friend bool operator!=(const Constant& lhs, const Constant& rhs) { return !(lhs == rhs); }

    static Constant CreateUtf8(std::string const& content)
    {
        Constant constant;
        constant.Type = TypeT::Utf8;
        constant.Utf8 = content;
        return constant;
    }

    static Constant CreateInt(IntT i)
    {
        Constant constant;
        constant.Type = TypeT::Integer;
        constant.Integer = i;
        return constant;
    }

    static Constant CreateFloat(FloatT float_)
    {
        Constant constant;
        constant.Type = TypeT::Float;
        constant.Float = float_;
        return constant;
    }

    static Constant CreateString(IdT Utf8)
    {
        Constant constant;
        constant.Type = TypeT::String;
        constant.Utf8Id = Utf8;
        return constant;
    }

    static Constant CreateNaT(IdT nameId, IdT typeId)
    {
        Constant constant;
        constant.Type = TypeT::NameAndType;
        constant.NameId = nameId;
        constant.TypeId = typeId;
        return constant;
    }

    static Constant CreateClass(IdT classNameId)
    {
        Constant constant;
        constant.Type = TypeT::Class;
        constant.ClassNameId = classNameId;
        return constant;
    }

    static Constant CreateFieldRef(IdT natId, IdT classId)
    {
        Constant constant;
        constant.Type = TypeT::FieldRef;
        constant.NameAndTypeId = natId;
        constant.ClassId = classId;
        return constant;
    }

    static Constant CreateMethodRef(IdT natId, IdT classId)
    {
        Constant constant;
        constant.Type = TypeT::MethodRef;
        constant.NameAndTypeId = natId;
        constant.ClassId = classId;
        return constant;
    }
};



struct ConstantTable
{
    std::vector<Constant> Constants;
    using ConstantRef = Constant const&;

    IdT FindUtf8(std::string const& utf8)
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

    IdT FindInt(IntT i)
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

    IdT FindFloat(FloatT i)
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

    IdT FindClass(std::string const& className)
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

    IdT FindNaT(std::string const& name, std::string const& type)
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

    IdT FindFieldRef(std::string const& className, std::string const& name, std::string const& type)
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

    IdT FindMethodRef(std::string const& className, std::string const& name, std::string const& type)
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
};

struct ClassAnalyzer
{
    MethodDeclNode* CurrentMethod = nullptr;
    ClassDeclNode* CurrentClass = nullptr;

    explicit ClassAnalyzer(ClassDeclNode* node) : CurrentClass{ node } { }

    void Analyze()
    {
        AnalyzeClass(CurrentClass);
    }

    void AnalyzeVarDecl(VarDeclNode* varDecl) const
    {
        if (!varDecl)
            return;
        varDecl->AType = varDecl->VarType->ToDataType();
        if (CurrentMethod) { CurrentMethod->variables.push_back(varDecl); }
        AnalyzeExpr(varDecl->InitExpr);
    }

    void AnalyzeWhile(WhileNode* while_)
    {
        if (!while_)
            return;
        AnalyzeExpr(while_->Condition);
        AnalyzeStmt(while_->Body);
    }

    void AnalyzeDoWhile(DoWhileNode* doWhile)
    {
        if (!doWhile)
            return;
        AnalyzeExpr(doWhile->Condition);
        AnalyzeStmt(doWhile->Body);
    }

    void AnalyzeFor(ForNode* for_)
    {
        if (!for_)
            return;
        AnalyzeVarDecl(for_->VarDecl);
        AnalyzeExpr(for_->FirstExpr);
        AnalyzeExpr(for_->Condition);
        AnalyzeExpr(for_->IterExpr);
        AnalyzeStmt(for_->Body);
    }

    void AnalyzeForEach(ForEachNode* forEach)
    {
        if (!forEach)
            return;
        AnalyzeVarDecl(forEach->VarDecl);
        AnalyzeExpr(forEach->Expr);
        AnalyzeStmt(forEach->Body);
    }

    void AnalyzeIf(IfNode* if_)
    {
        if (!if_)
            return;
        AnalyzeExpr(if_->Condition);
        AnalyzeStmt(if_->ThenBranch);
        AnalyzeStmt(if_->ElseBranch);
    }

    void AnalyzeStmt(StmtNode* stmt)
    {
        AnalyzeVarDecl(stmt->VarDecl);
        AnalyzeWhile(stmt->While);
        AnalyzeDoWhile(stmt->DoWhile);
        AnalyzeFor(stmt->For);
        AnalyzeForEach(stmt->ForEach);
        AnalyzeIf(stmt->If);
        AnalyzeExpr(stmt->Expr);
    }

    void AnalyzeMethod(MethodDeclNode* method)
    {
        CurrentMethod = method;
        for (auto* stmt : method->Body->GetSeq()) { AnalyzeStmt(stmt); }
        CurrentMethod = nullptr;
    }

    void AnalyzeField(FieldDeclNode* field) const
    {
        AnalyzeVarDecl(field->VarDecl);
    }

    void AnalyzeClass(ClassDeclNode* value)
    {
        for (auto* field : value->Members->Fields) { AnalyzeField(field); }
        for (auto* method : value->Members->Methods) { AnalyzeMethod(method); }
    }



    void AnalyzeExpr(ExprNode* expr) const { CalculateTypesForExpr(expr); }

    void CalculateTypesForExpr(ExprNode* node) const
    {
        if (!node)
            return;
        if (node->Type == ExprNode::TypeT::AccessExpr)
        {
            // ReSharper disable once CppIncompleteSwitchStatement
            // ReSharper disable once CppDefaultCaseNotHandledInSwitchStatement
            switch (node->Access->Type)  // NOLINT(clang-diagnostic-switch)
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
                if (CurrentMethod)
                {
                    if (auto* var = CurrentMethod->FindVariableByName(node->Access->Identifier); var)
                    {
                        node->AType = var->AType;
                    }
                }

                if (CurrentClass)
                {
                    if (auto* var = CurrentClass->FindFieldByName(node->Access->Identifier); var)
                    {
                        node->AType = var->VarDecl->AType;
                    }
                }
            }
            }
            return;
        }

        if (IsBinary(node->Type))
        {
            CalculateTypesForExpr(node->Left);
            CalculateTypesForExpr(node->Right);
            if (node->Left->AType == node->Right->AType) { node->AType = node->Left->AType; }
            return;
        }

        node->AType.IsUnknown = true;
    }
};

