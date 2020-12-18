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
        Utf8 = 1,
        Integer = 3,
        Float = 4,
        String = 8,
        NameAndType = 12,
        Class = 7,
        MethodRef = 9,
        FieldRef = 10
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

    friend bool operator==(const Constant& lhs, const Constant& rhs);

    friend bool operator!=(const Constant& lhs, const Constant& rhs) { return !(lhs == rhs); }

    static Constant CreateUtf8(std::string const& content);

    static Constant CreateInt(IntT i);

    static Constant CreateFloat(FloatT float_);

    static Constant CreateString(IdT Utf8);

    static Constant CreateNaT(IdT nameId, IdT typeId);

    static Constant CreateClass(IdT classNameId);

    static Constant CreateFieldRef(IdT natId, IdT classId);

    static Constant CreateMethodRef(IdT natId, IdT classId);
};

struct ConstantTable
{
    std::vector<Constant> Constants;
    using ConstantRef = Constant const&;

    IdT FindUtf8(std::string const& utf8);

    IdT FindInt(IntT i);

    IdT FindFloat(FloatT i);

    IdT FindClass(std::string const& className);

    IdT FindNaT(std::string const& name, std::string const& type);

    IdT FindFieldRef(std::string const& className, std::string const& name, std::string const& type);

    IdT FindMethodRef(std::string const& className, std::string const& name, std::string const& type);
};

struct ClassAnalyzer
{
    MethodDeclNode* CurrentMethod = nullptr;
    ClassDeclNode* CurrentClass = nullptr;
    ConstantTable Table;
    std::vector<std::string> Errors;

    explicit ClassAnalyzer(ClassDeclNode* node);

    void Analyze();

    void AnalyzeVarDecl(VarDeclNode* varDecl);

    void AnalyzeWhile(WhileNode* while_);

    void AnalyzeDoWhile(DoWhileNode* doWhile);

    void AnalyzeFor(ForNode* for_);

    void AnalyzeForEach(ForEachNode* forEach);

    void AnalyzeIf(IfNode* if_);

    void AnalyzeStmt(StmtNode* stmt);

    void AnalyzeMethod(MethodDeclNode* method);

    void AnalyzeField(FieldDeclNode* field);

    void AnalyzeClass(ClassDeclNode* value);

    [[nodiscard]] ExprNode* AnalyzeExpr(ExprNode* expr);

    void CalculateTypesForExpr(ExprNode* node);

    DataType CalculateTypeForAccessExpr(AccessExpr* access);

    static ExprNode* ReplaceAssignmentsOnArrayElements(ExprNode* node);
};
