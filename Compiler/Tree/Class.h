#pragma once
#include <algorithm>
#include <iterator>


#include "Stmt.h"
#include "Type.h"
#include "../VisibilityModifier.h"

struct ClassDeclNode;

struct FieldDeclNode final : Node
{
    const VisibilityModifier Visibility;
    VarDeclNode* VarDecl;
    ClassDeclNode* Class{};

    ExprNode* InitInConstructor{};

    bool IsFinal{};

    FieldDeclNode(const VisibilityModifier visibility, VarDeclNode* const varDecl)
        : Visibility{ visibility }
      , VarDecl{ varDecl }
    {
    }


    [[nodiscard]] std::string_view Name() const noexcept override { return "FieldDeclNode"; }
};

struct MethodArguments final : NodeSeq<MethodArguments, VarDeclNode>
{
    using NodeSeq<MethodArguments, VarDeclNode>::NodeSeq;
    [[nodiscard]] std::string_view Name() const noexcept override { return "MethodArguments"; }
};

struct MethodArgumentDto
{
    DataType Type;
    std::string Name;
};

inline std::vector<DataType> ToTypes(std::vector<MethodArgumentDto> const& arguments)
{
    std::vector<DataType> types(arguments.size());
    std::transform(arguments.begin(), arguments.end(), types.begin(), [](auto& arg) { return arg.Type; });
    return types;
}

inline MethodArgumentDto ToMethodArgumentDto(VarDeclNode* node)
{
    return {
        ToDataType(node->VarType),
        std::string{ node->Identifier }
    };
}

enum class OperatorType
{
    Plus,
    Minus,
    Multiply,
    Divide,
    Less,
    Greater,
    Equal,
    NotEqual,
    GreaterOrEqual,
    LessOrEqual,
    Not,
    UnaryMinus,
    UnaryPlus,
    Increment,
    Decrement
};

inline OperatorType ToOperatorOverload(const ExprNode::TypeT type)
{
    // ReSharper disable once CppDefaultCaseNotHandledInSwitchStatement
    // ReSharper disable once CppIncompleteSwitchStatement
    switch (type) // NOLINT(clang-diagnostic-switch)
    {
        case ExprNode::TypeT::BinPlus:
            return OperatorType::Plus;
        case ExprNode::TypeT::BinMinus:
            return OperatorType::Minus;
        case ExprNode::TypeT::Multiply:
            return OperatorType::Multiply;
        case ExprNode::TypeT::Divide:
            return OperatorType::Divide;
        case ExprNode::TypeT::Less:
            return OperatorType::Less;
        case ExprNode::TypeT::Greater:
            return OperatorType::Greater;
        case ExprNode::TypeT::Equal:
            return OperatorType::Equal;
        case ExprNode::TypeT::NotEqual:
            return OperatorType::NotEqual;
        case ExprNode::TypeT::GreaterOrEqual:
            return OperatorType::GreaterOrEqual;
        case ExprNode::TypeT::LessOrEqual:
            return OperatorType::LessOrEqual;
        case ExprNode::TypeT::Not:
            return OperatorType::Not;
        case ExprNode::TypeT::UnaryMinus:
            return OperatorType::UnaryMinus;
        case ExprNode::TypeT::UnaryPlus:
            return OperatorType::UnaryPlus;
        case ExprNode::TypeT::Increment:
            return OperatorType::Increment;
        case ExprNode::TypeT::Decrement:
            return OperatorType::Decrement;
    }
    return {};
}

struct MethodDeclNode final : Node
{
    const VisibilityModifier Visibility{};
    const TypeNode* Type{};
private:
    std::string_view _identifier{};
public:
    MethodArguments* Arguments{};
    StmtSeqNode* Body{};
    const bool IsStatic{};
    const bool IsConstructor{};
    std::vector<VarDeclNode*> Variables{};
    DataType AReturnType{};
    std::vector<MethodArgumentDto> ArgumentDtos{};

    ClassDeclNode* Class{};

    bool IsOperatorOverload = false;
    OperatorType Operator{};

    VarDeclNode* FindVariableByName(std::string_view var, int scopingLevel)
    {
        for (auto* variable : Variables)
        {
            if (variable->Identifier == var && variable->ScopingLevel <= scopingLevel)
                return variable;
        }
        return nullptr;
    }

    MethodDeclNode(const VisibilityModifier visibility, const TypeNode* const type, const std::string_view identifier,
                   MethodArguments* const arguments, StmtSeqNode* const body, const bool isStatic = false)
        : Visibility{ visibility }
      , Type{ type }
      , _identifier{ identifier }
      , Arguments{ arguments }
      , Body{ body }
      , IsStatic{ isStatic }
      , IsConstructor{ _identifier == "<init>" }
    {
    }

    MethodDeclNode(const VisibilityModifier visibility, const TypeNode* const returnType, const OperatorType operator_,
                   VarDeclNode* lhsArg, VarDeclNode* rhsArg, StmtSeqNode* const body)
        : Visibility{ visibility }
      , Type{ returnType }
      , Arguments{ new MethodArguments }
      , Body{ body }
      , IsStatic{ true }
      , IsOperatorOverload{ true }
      , Operator{ operator_ }
    {
        Arguments->Add(lhsArg);
        Arguments->Add(rhsArg);
    }

    MethodDeclNode(const VisibilityModifier visibility, const TypeNode* const returnType, const OperatorType operator_,
                   VarDeclNode* arg, StmtSeqNode* const body)
        : Visibility{ visibility }
      , Type{ returnType }
      , Arguments{ new MethodArguments }
      , Body{ body }
      , IsStatic{ true }
      , IsOperatorOverload{ true }
      , Operator{ operator_ } { Arguments->Add(arg); }

    [[nodiscard]] std::string_view Name() const noexcept override { return "MethodDecl"; }

    [[nodiscard]] std::string ToDescriptor() const
    {
        std::string desc = "(";
        for (const auto& param : ArgumentDtos) { desc += param.Type.ToDescriptor(); }
        desc += ")";
        desc += AReturnType.ToDescriptor();
        return desc;
    }

    void AnalyzeArguments()
    {
        auto&& varDeclNodes = Arguments->GetSeq();
        std::transform(varDeclNodes.begin(), varDeclNodes.end(),
                       std::back_inserter(ArgumentDtos),
                       ToMethodArgumentDto);
    }

    [[nodiscard]] std::string Identifier() const
    {
        if (!IsOperatorOverload)
            return std::string{ _identifier };

        switch (Operator)
        {
            case OperatorType::Plus:
                return "__operator_plus";
            case OperatorType::Minus:
                return "__operator_minus";
            case OperatorType::Multiply:
                return "__operator_multiply";
            case OperatorType::Divide:
                return "__operator_divide";
            case OperatorType::Less:
                return "__operator_less";
            case OperatorType::Greater:
                return "__operator_greater";
            case OperatorType::Equal:
                return "__operator_equal";
            case OperatorType::NotEqual:
                return "__operator_not_equal";
            case OperatorType::GreaterOrEqual:
                return "__operator_greater_or_equal";
            case OperatorType::LessOrEqual:
                return "__operator_less_or_equal";
            case OperatorType::Not:
                return "__operator_not";
            case OperatorType::UnaryMinus:
                return "__operator_minus";
            case OperatorType::UnaryPlus:
                return "__operator_plus";
            case OperatorType::Increment:
                return "__operator_increment";
            case OperatorType::Decrement:
                return "__operator_decrement";
            default: ;
        }
        return {};
    }

    friend struct ClassAnalyzer;
};

struct ClassMembersNode final : Node
{
    std::vector<MethodDeclNode*> Methods;
    std::vector<FieldDeclNode*> Fields;

    void Add(MethodDeclNode* node) { Methods.push_back(node); }
    void Add(FieldDeclNode* node) { Fields.push_back(node); }

    ClassMembersNode() noexcept = default;

    [[nodiscard]] std::string_view Name() const noexcept override { return "ClassMembers"; }
};

struct NamespaceDeclNode;

struct ClassDeclNode final : Node
{
    std::string_view ClassName;
    IdentifierList* ParentType;
    ClassMembersNode* Members;
    NamespaceDeclNode* Namespace{};

    MethodDeclNode* Constructor{};

    [[nodiscard]] DataType ToDataType() const;

    [[nodiscard]] FieldDeclNode* FindFieldByName(std::string_view name) const
    {
        for (auto* field : Members->Fields)
        {
            if (field->VarDecl->Identifier == name)
                return field;
        }
        return nullptr;
    }


    ClassDeclNode(const std::string_view className, IdentifierList* parentType, ClassMembersNode* const members)
        : ClassName{ className }
      , ParentType{ parentType }
      , Members{ members }
    {
    }

    [[nodiscard]] std::string_view Name() const noexcept override { return "ClassDecl"; }
};
