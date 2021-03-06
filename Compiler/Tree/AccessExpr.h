#pragma once

#include "Expr.h"

struct MethodDeclNode;
struct FieldDeclNode;
struct VarDeclNode;

struct AccessExpr final : Node
{
    enum class TypeT
    {
        Expr,
        ArrayElementExpr,
        ComplexArrayType,
        Integer,
        Float,
        String,
        Char,
        Bool,
        Identifier,
        SimpleMethodCall,
        Dot,
        DotMethodCall,
        ArrayLength
    } Type{};

    ExprNode* Child{};

    AccessExpr* Previous{};

    int Integer{};
    double Float{};
    char Char{};
    std::string_view String{};
    std::string_view Identifier{};
    bool Bool{};

    ExprSeqNode* Arguments{};

    MethodDeclNode* ActualMethodCall{};
    FieldDeclNode* ActualField{};
    VarDeclNode* ActualVar{};

    DataType AType{};

    [[nodiscard]] std::string_view Name() const noexcept override { return "AccessExpr"; }

    static AccessExpr* FromExpr(ExprNode* expr);

    static AccessExpr* FromBrackets(AccessExpr* previous);

    static AccessExpr* FromBrackets(AccessExpr* previous, ExprNode* child);

    static AccessExpr* FromInt(int value);

    static AccessExpr* FromFloat(double value);

    static AccessExpr* FromString(const char* str);

    static AccessExpr* FromId(const char* id);

    static AccessExpr* FromBool(bool value);

    static AccessExpr* FromChar(char value);

    static AccessExpr* FromCall(const char* id, ExprSeqNode* arguments);

    static AccessExpr* FromDot(AccessExpr* previous, const char* id);

    static AccessExpr* FromDot(AccessExpr* previous, const char* id, ExprSeqNode* arguments);

    [[nodiscard]] DataType ToDataType() const;

    void CallForAllChildren(const std::function<void(AccessExpr*)>& function) const;

private:
    AccessExpr() : Node()
    {
    }
};
