#include "Semantic.h"

NamespaceDeclNode* Semantic::CreateSystemNamespace() const
{
    std::vector<ClassDeclNode*> classes{ CreateConsoleClass(), CreateStringClass() };

    auto* systemMembers = new NamespaceMembersNode();
    for (auto* class_ : classes) { systemMembers->Add(class_); }
    auto* systemNamespace = new NamespaceDeclNode("System", systemMembers);
    for (auto* class_ : classes)
    {
        class_->Namespace = systemNamespace;
        ClassAnalyzer analyzer(class_, systemNamespace, program->Namespaces); // Создаёт конструктор по умолчанию
        analyzer.AnalyzeMemberSignatures();
    }
    return systemNamespace;
}

ClassDeclNode* Semantic::CreateConsoleClass()
{
    auto* consoleClassMembers = new ClassMembersNode();
    auto& consoleMethods = consoleClassMembers->Methods;

    [[maybe_unused]] auto* readIntMethod = [&]
    {
        auto* method = new MethodDeclNode(VisibilityModifier::Public, nullptr, "ReadInt",
                                          MethodArguments::MakeEmpty(), nullptr);
        method->AReturnType = DataType::IntType;
        method->AnalyzeArguments();
        consoleMethods.push_back(method);
        return method;
    }();
    [[maybe_unused]] auto* writeLineIntMethod = [&]
    {
        auto* arg = new VarDeclNode(new TypeNode(StandardType::Int), "arg", nullptr);
        auto* args = MethodArguments::MakeEmpty();
        args->Add(arg);
        auto* method = new MethodDeclNode(VisibilityModifier::Public, nullptr, "WriteLine", args, nullptr);
        method->AReturnType = DataType::VoidType;
        method->AnalyzeArguments();
        consoleMethods.push_back(method);
        return method;
    }();

    [[maybe_unused]] auto* writeIntMethod = [&]
    {
        auto* arg = new VarDeclNode(new TypeNode(StandardType::Int), "arg", nullptr);
        auto* args = MethodArguments::MakeEmpty();
        args->Add(arg);
        auto* method = new MethodDeclNode(VisibilityModifier::Public, nullptr, "Write", args, nullptr);
        method->AReturnType = DataType::VoidType;
        method->AnalyzeArguments();
        consoleMethods.push_back(method);
        return method;
    }();

    [[maybe_unused]] auto* writeLineBoolMethod = [&]
    {
        auto* arg = new VarDeclNode(new TypeNode(StandardType::Bool), "arg", nullptr);
        auto* args = MethodArguments::MakeEmpty();
        args->Add(arg);
        auto* method = new MethodDeclNode(VisibilityModifier::Public, nullptr, "WriteLine", args, nullptr);
        method->AReturnType = DataType::VoidType;
        method->AnalyzeArguments();
        consoleMethods.push_back(method);
        return method;
    }();

    [[maybe_unused]] auto* writeLineStrMethod = [&]
    {
        auto* arg = new VarDeclNode(new TypeNode(StandardType::String), "arg", nullptr);
        auto* args = MethodArguments::MakeEmpty();
        args->Add(arg);
        auto* method = new MethodDeclNode(VisibilityModifier::Public, nullptr, "WriteLine", args, nullptr);
        method->AReturnType = DataType::VoidType;
        method->AnalyzeArguments();
        consoleMethods.push_back(method);
        return method;
    }();

    [[maybe_unused]] auto* writeStrMethod = [&]
    {
        auto* arg = new VarDeclNode(new TypeNode(StandardType::String), "arg", nullptr);
        auto* args = MethodArguments::MakeEmpty();
        args->Add(arg);
        auto* method = new MethodDeclNode(VisibilityModifier::Public, nullptr, "Write", args, nullptr);
        method->AReturnType = DataType::VoidType;
        method->AnalyzeArguments();
        consoleMethods.push_back(method);
        return method;
    }();

    [[maybe_unused]] auto* writeLineCharMethod = [&]
    {
        auto* arg = new VarDeclNode(new TypeNode(StandardType::Char), "arg", nullptr);
        auto* args = MethodArguments::MakeEmpty();
        args->Add(arg);
        auto* method = new MethodDeclNode(VisibilityModifier::Public, nullptr, "WriteLine", args, nullptr);
        method->AReturnType = DataType::VoidType;
        method->AnalyzeArguments();
        consoleMethods.push_back(method);
        return method;
    }();

    [[maybe_unused]] auto* writeCharMethod = [&]
    {
        auto* arg = new VarDeclNode(new TypeNode(StandardType::Char), "arg", nullptr);
        auto* args = MethodArguments::MakeEmpty();
        args->Add(arg);
        auto* method = new MethodDeclNode(VisibilityModifier::Public, nullptr, "Write", args, nullptr);
        method->AReturnType = DataType::VoidType;
        method->AnalyzeArguments();
        consoleMethods.push_back(method);
        return method;
    }();

    auto* consoleClass = new ClassDeclNode("Console", nullptr, consoleClassMembers);

    for (auto* method : consoleMethods) { method->Class = consoleClass; }

    return consoleClass;
}

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
