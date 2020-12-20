#pragma once
#include "Expr.h"
#include "Type.h"
#include "../Semantic/JvmClass.h"

struct VarDeclNode final : Node
{
    TypeNode* VarType{};
    std::string_view Identifier{};
    ExprNode* InitExpr{};

    DataType AType{};
    int PositionInMethod{};

    VarDeclNode(TypeNode* const varType, const char* const identifier, ExprNode* const initExpr)
        : VarType{ varType }
      , Identifier{ identifier }
      , InitExpr{ initExpr }
    {
    }


    [[nodiscard]] std::string_view Name() const noexcept override
    {
        if (!InitExpr)
            return "VarDecl";
        return "VarDeclWithInit";
    }
};

struct StmtNode;

struct WhileNode final : Node
{
    ExprNode* Condition;
    StmtNode* Body;


    WhileNode(ExprNode* const condition, StmtNode* const body)
        : Condition{ condition }
      , Body{ body }
    {
    }

    [[nodiscard]] std::string_view Name() const noexcept override { return "While"; }
};

struct DoWhileNode final : Node
{
    ExprNode* Condition;
    StmtNode* Body;

    DoWhileNode(ExprNode* const condition, StmtNode* const body)
        : Condition{ condition }
      , Body{ body }
    {
    }

    [[nodiscard]] std::string_view Name() const noexcept override { return "DoWhile"; }
};

struct ForNode final : Node
{
    VarDeclNode* VarDecl{};
    ExprNode* FirstExpr{};
    ExprNode* Condition{};
    ExprNode* IterExpr{};
    StmtNode* Body{};


    ForNode(VarDeclNode* const varDecl, ExprNode* const condition, ExprNode* const iterExpr, StmtNode* const body)
        : VarDecl{ varDecl }
      , Condition{ condition }
      , IterExpr{ iterExpr }
      , Body{ body }
    {
    }


    ForNode(ExprNode* const firstExpr, ExprNode* const condition, ExprNode* const iterExpr, StmtNode* const body)
        : FirstExpr{ firstExpr }
      , Condition{ condition }
      , IterExpr{ iterExpr }
      , Body{ body }
    {
    }

    [[nodiscard]] std::string_view Name() const noexcept override { return "For"; }
};

struct ForEachNode final : Node
{
    VarDeclNode* VarDecl;
    ExprNode* Expr;
    StmtNode* Body;

    ForEachNode(VarDeclNode* const varDecl, ExprNode* const expr, StmtNode* const stmt)
        : VarDecl{ varDecl }
      , Expr{ expr }
      , Body{ stmt }
    {
    }


    [[nodiscard]] std::string_view Name() const noexcept override { return "Foreach"; }
};

struct StmtSeqNode final : NodeSeq<StmtSeqNode, StmtNode>
{
    using NodeSeq<StmtSeqNode, StmtNode>::NodeSeq;

    [[nodiscard]] std::string_view Name() const noexcept override
    {
        if (IsEmpty())
            return "Empty StmtSeq";
        return "StmtSeq";
    }
};

struct IfNode : Node
{
    ExprNode* Condition{};
    StmtNode* ThenBranch{};
    StmtNode* ElseBranch{};

    IfNode(ExprNode* const condition, StmtNode* const thenBranch, StmtNode* const elseBranch = nullptr)
        : Condition{ condition }
      , ThenBranch{ thenBranch }
      , ElseBranch{ elseBranch }
    {
    }

    [[nodiscard]] std::string_view Name() const noexcept override { return "If"; }
};

struct StmtNode final : Node
{
    enum class TypeT
    {
        Empty,
        VarDecl,
        While,
        DoWhile,
        For,
        Foreach,
        BlockStmt,
        IfStmt,
        Return,
        ExprStmt
    } Type;

    VarDeclNode* VarDecl{};
    WhileNode* While{};
    DoWhileNode* DoWhile{};
    ForNode* For{};
    ForEachNode* ForEach{};
    StmtSeqNode* Block{};
    IfNode* If{};
    ExprNode* Expr{}; // For ExprStmt or Return


    explicit StmtNode(VarDeclNode* const varDecl)
        : Type{ TypeT::VarDecl }
      , VarDecl{ varDecl }
    {
    }


    explicit StmtNode(WhileNode* const while_)
        : Type{ TypeT::While }
      , While{ while_ }
    {
    }


    explicit StmtNode(DoWhileNode* const doWhile)
        : Type{ TypeT::DoWhile }
      , DoWhile{ doWhile }
    {
    }

    explicit StmtNode(ForNode* const for_)
        : Type{ TypeT::For }
      , For{ for_ }
    {
    }

    explicit StmtNode(ForEachNode* const forEach)
        : Type{ TypeT::Foreach }
      , ForEach{ forEach }
    {
    }


    explicit StmtNode(StmtSeqNode* const block)
        : Type{ TypeT::BlockStmt }
      , Block{ block }
    {
    }


    explicit StmtNode(IfNode* const if_)
        : Type{ TypeT::IfStmt }
      , If{ if_ }
    {
    }


    explicit StmtNode(ExprNode* const expr, const bool isReturn)
        : Type{ isReturn ? TypeT::Return : TypeT::ExprStmt }
      , Expr{ expr }
    {
    }


    StmtNode()
        : Type{ TypeT::Empty }
    {
    }

    [[nodiscard]] std::string_view Name() const noexcept override { return "Stmt"; }
};
