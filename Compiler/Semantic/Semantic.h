#pragma once
#include <vector>
#include <set>
#include "JvmClass.h"
#include "ClassAnalyzer.h"

struct Semantic
{
    // ReSharper disable once CppInconsistentNaming
    Program* program;
    std::vector<Class> Classes{};
    std::vector<MethodDeclNode*> AllMains{};

    explicit Semantic(Program* program) : program{ program }
    {
    }

    MethodDeclNode* CurrentMethod = nullptr;
    ClassDeclNode* CurrentClass = nullptr;

    std::set<std::string> Errors;

    void Analyze()
    {
        CheckSystemNamespace();
        for (auto* _namespace : program->Namespaces->GetSeq())
        {
            if (_namespace->NamespaceName != "System") { AnalyzeNamespace(_namespace); }
        }
        if (AllMains.size() > 1) { Errors.insert("There must be only one main in the program"); }
        if (AllMains.empty()) { Errors.insert("Cannot run a program without an entry point"); }
    }

    void CheckSystemNamespace()
    {
        for (const auto* _namespace : program->Namespaces->GetSeq())
        {
            if (_namespace->NamespaceName == "System")
            {
                Errors.insert("You cannot declare namespace System");
                return;
            }
        }

        auto* readIntMethod = []
        {
            auto* method = new MethodDeclNode(VisibilityModifier::Public, nullptr, "ReadInt",
                                              MethodArguments::MakeEmpty(), nullptr);
            method->AReturnType = DataType::IntType;
            method->AnalyzeArguments();
            return method;
        }();
        auto* writeLineIntMethod = []
        {
            auto* arg = new VarDeclNode(new TypeNode(StandardType::Int), "arg", nullptr);
            auto* args = MethodArguments::MakeEmpty();
            args->Add(arg);
            auto* method = new MethodDeclNode(VisibilityModifier::Public, nullptr, "WriteLine", args, nullptr);
            method->AReturnType = DataType::VoidType;
            method->AnalyzeArguments();
            return method;
        }();

        auto* writeIntMethod = []
        {
            auto* arg = new VarDeclNode(new TypeNode(StandardType::Int), "arg", nullptr);
            auto* args = MethodArguments::MakeEmpty();
            args->Add(arg);
            auto* method = new MethodDeclNode(VisibilityModifier::Public, nullptr, "Write", args, nullptr);
            method->AReturnType = DataType::VoidType;
            method->AnalyzeArguments();
            return method;
        }();

        auto* writeLineBoolMethod = []
        {
            auto* arg = new VarDeclNode(new TypeNode(StandardType::Bool), "arg", nullptr);
            auto* args = MethodArguments::MakeEmpty();
            args->Add(arg);
            auto* method = new MethodDeclNode(VisibilityModifier::Public, nullptr, "WriteLine", args, nullptr);
            method->AReturnType = DataType::VoidType;
            method->AnalyzeArguments();
            return method;
        }();

        auto* writeLineStrMethod = []
        {
            auto* arg = new VarDeclNode(new TypeNode(StandardType::String), "arg", nullptr);
            auto* args = MethodArguments::MakeEmpty();
            args->Add(arg);
            auto* method = new MethodDeclNode(VisibilityModifier::Public, nullptr, "WriteLine", args, nullptr);
            method->AReturnType = DataType::VoidType;
            method->AnalyzeArguments();
            return method;
        }();

        auto* writeStrMethod = []
        {
            auto* arg = new VarDeclNode(new TypeNode(StandardType::String), "arg", nullptr);
            auto* args = MethodArguments::MakeEmpty();
            args->Add(arg);
            auto* method = new MethodDeclNode(VisibilityModifier::Public, nullptr, "Write", args, nullptr);
            method->AReturnType = DataType::VoidType;
            method->AnalyzeArguments();
            return method;
        }();

        auto* writeLineCharMethod = []
        {
            auto* arg = new VarDeclNode(new TypeNode(StandardType::Char), "arg", nullptr);
            auto* args = MethodArguments::MakeEmpty();
            args->Add(arg);
            auto* method = new MethodDeclNode(VisibilityModifier::Public, nullptr, "WriteLine", args, nullptr);
            method->AReturnType = DataType::VoidType;
            method->AnalyzeArguments();
            return method;
        }();

        auto* consoleClassMembers = new ClassMembersNode();
        consoleClassMembers->Methods.push_back(readIntMethod);
        consoleClassMembers->Methods.push_back(writeLineIntMethod);
        consoleClassMembers->Methods.push_back(writeIntMethod);
        consoleClassMembers->Methods.push_back(writeLineStrMethod);
        consoleClassMembers->Methods.push_back(writeStrMethod);
        consoleClassMembers->Methods.push_back(writeLineBoolMethod);
        consoleClassMembers->Methods.push_back(writeLineCharMethod);


        auto* consoleClass = new ClassDeclNode("Console", nullptr, consoleClassMembers);

        readIntMethod->Class = consoleClass;
        writeLineIntMethod->Class = consoleClass;
        writeIntMethod->Class = consoleClass;
        writeLineBoolMethod->Class = consoleClass;
        writeLineStrMethod->Class = consoleClass;
        writeStrMethod->Class = consoleClass;
        writeLineCharMethod->Class = consoleClass;

        auto* systemMembers = new NamespaceMembersNode();
        systemMembers->Add(consoleClass);
        auto* systemNamespace = new NamespaceDeclNode("System", systemMembers);
        consoleClass->Namespace = systemNamespace;
        program->Namespaces->Add(systemNamespace);

        for (auto* class_ : systemMembers->Classes)
        {
            ClassAnalyzer analyzer(class_, nullptr, nullptr); // Создаёт конструктор по умолчанию
        }
    }

    void AnalyzeNamespace(NamespaceDeclNode* namespace_)
    {
        for (auto* class_ : namespace_->Members->Classes)
        {
            ClassAnalyzer analyzer(class_, namespace_, program->Namespaces);
            analyzer.AnalyzeMemberSignatures();
        }
        for (auto* class_ : namespace_->Members->Classes)
        {
            ClassAnalyzer analyzer(class_, namespace_, program->Namespaces);
            analyzer.Analyze();
            Errors.insert(analyzer.Errors.begin(), analyzer.Errors.end());
            AllMains.insert(AllMains.end(), analyzer.AllMains.begin(), analyzer.AllMains.end());
        }
    } // TODO enums

    void Generate() const
    {
        if (Errors.empty())
        {
            for (auto* namespace_ : program->Namespaces->GetSeq())
            {
                if (namespace_->NamespaceName != "System")
                {
                    for (auto* class_ : namespace_->Members->Classes)
                    {
                        ClassAnalyzer analyzer(class_, namespace_, program->Namespaces);
                        analyzer.FillTables();
                        analyzer.Generate();
                    }
                }
            }
        }
    }
};
