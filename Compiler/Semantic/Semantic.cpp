#include "Semantic.h"
ClassDeclNode* Semantic::CreateStringClass()
{
    auto* members = new ClassMembersNode();
    auto& methods = members->Methods;
    [[maybe_unused]] auto* charAtMethod = [&]
    {
        auto* arg = new VarDeclNode(new TypeNode(StandardType::Int), "arg", nullptr);
        auto* args = MethodArguments::MakeEmpty();
        args->Add(arg);
        auto* method = new MethodDeclNode(VisibilityModifier::Public, nullptr, "CharAt",
                                          args, nullptr);
        method->AReturnType = DataType::CharType;
        method->AnalyzeArguments();
        methods.push_back(method);
        return method;
    }();
    [[maybe_unused]] auto* equalsMethod = [&]
    {
        auto* arg = new VarDeclNode(new TypeNode(StandardType::String), "arg", nullptr);
        auto* args = MethodArguments::MakeEmpty();
        args->Add(arg);
        auto* method = new MethodDeclNode(VisibilityModifier::Public, nullptr, "Equals",
                                          args, nullptr);
        method->AReturnType = DataType::BoolType;
        method->AnalyzeArguments();
        methods.push_back(method);
        return method;
    }();
    [[maybe_unused]] auto* substring1Method = [&]
    {
        auto* arg = new VarDeclNode(new TypeNode(StandardType::Int), "startIndex", nullptr);
        auto* args = MethodArguments::MakeEmpty();
        args->Add(arg);
        auto* method = new MethodDeclNode(VisibilityModifier::Public, nullptr, "Substring",
                                          args, nullptr);
        method->AReturnType = STD_STRING_TYPE;
        method->AnalyzeArguments();
        methods.push_back(method);
        return method;
    }();
    [[maybe_unused]] auto* substring2Method = [&]
    {
        auto* arg1 = new VarDeclNode(new TypeNode(StandardType::Int), "startIndex", nullptr);
        auto* arg2 = new VarDeclNode(new TypeNode(StandardType::Int), "length", nullptr);
        auto* args = MethodArguments::MakeEmpty();
        args->Add(arg1);
        args->Add(arg2);
        auto* method = new MethodDeclNode(VisibilityModifier::Public, nullptr, "Substring",
                                          args, nullptr);
        method->AReturnType = STD_STRING_TYPE;
        method->AnalyzeArguments();
        methods.push_back(method);
        return method;
    }();
    [[maybe_unused]] auto* indexOf1Method = [&]
    {
        auto* arg = new VarDeclNode(new TypeNode(StandardType::Char), "arg", nullptr);
        auto* args = MethodArguments::MakeEmpty();
        args->Add(arg);
        auto* method = new MethodDeclNode(VisibilityModifier::Public, nullptr, "IndexOf",
                                          args, nullptr);
        method->AReturnType = DataType::IntType;
        method->AnalyzeArguments();
        methods.push_back(method);
        return method;
    }();
    [[maybe_unused]] auto* indexOf2Method = [&]
    {
        auto* arg = new VarDeclNode(new TypeNode(StandardType::String), "arg", nullptr);
        auto* args = MethodArguments::MakeEmpty();
        args->Add(arg);
        auto* method = new MethodDeclNode(VisibilityModifier::Public, nullptr, "IndexOf",
                                          args, nullptr);
        method->AReturnType = DataType::IntType;
        method->AnalyzeArguments();
        methods.push_back(method);
        return method;
    }();
    [[maybe_unused]] auto* operatorPlus = [&]
    {
        auto* arg1 = new VarDeclNode(new TypeNode(StandardType::String), "lhs", nullptr);
        auto* arg2 = new VarDeclNode(new TypeNode(StandardType::String), "rhs", nullptr);
        auto* method = new MethodDeclNode(VisibilityModifier::Public, nullptr, OperatorType::Plus,
                                          arg1, arg2, nullptr);
        method->AReturnType = STD_STRING_TYPE;
        method->AnalyzeArguments();
        methods.push_back(method);
        return method;
    }();

    [[maybe_unused]] auto* lengthField = [&]
    {
        auto* field = new FieldDeclNode(VisibilityModifier::Public,
                                        new VarDeclNode(new TypeNode(StandardType::Int), "Length", nullptr));
        field->IsFinal = true;
        members->Fields.push_back(field);
        field->VarDecl->AType = ToDataType(field->VarDecl->VarType);
        return field;
    }();

    auto* stringClass = new ClassDeclNode("String", nullptr, members);

    for (auto* method : methods) { method->Class = stringClass; }

    return stringClass;
}
