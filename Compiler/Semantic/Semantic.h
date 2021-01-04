#pragma once
#include <vector>
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

    std::vector<std::string> Errors;

    void Analyze()
    {
        CheckSystemNamespace();
        for (auto* _namespace : program->Namespaces->GetSeq())
        {
            if (_namespace->NamespaceName != "System") { AnalyzeNamespace(_namespace); }
        }
        if (AllMains.size() > 1) { Errors.emplace_back("There must be only one main in the program"); }
        if (AllMains.empty()) { Errors.emplace_back("Cannot run a program without an entry point"); }
    }

    void CheckSystemNamespace()
    {
        for (const auto* _namespace : program->Namespaces->GetSeq())
        {
            if (_namespace->NamespaceName == "System")
            {
                Errors.emplace_back("You cannot declare namespace System");
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
        auto* writeIntMethod = []
        {
            auto* arg = new VarDeclNode(new TypeNode(StandardType::Int), "arg", nullptr);
            auto args = MethodArguments::MakeEmpty();
            args->Add(arg);
            auto* method = new MethodDeclNode(VisibilityModifier::Public, nullptr, "WriteLine", args, nullptr);
            method->AReturnType = DataType::VoidType;
            method->AnalyzeArguments();
            return method;
        }();
        auto* writeBoolMethod = []
        {
            auto* arg = new VarDeclNode(new TypeNode(StandardType::Bool), "arg", nullptr);
            auto args = MethodArguments::MakeEmpty();
            args->Add(arg);
            auto* method = new MethodDeclNode(VisibilityModifier::Public, nullptr, "WriteLine", args, nullptr);
            method->AReturnType = DataType::VoidType;
            method->AnalyzeArguments();
            return method;
        }();

        auto* writeStrMethod = []
        {
            auto* arg = new VarDeclNode(new TypeNode(StandardType::String), "arg", nullptr);
            auto args = MethodArguments::MakeEmpty();
            args->Add(arg);
            auto* method = new MethodDeclNode(VisibilityModifier::Public, nullptr, "WriteLine", args, nullptr);
            method->AReturnType = DataType::VoidType;
            method->AnalyzeArguments();
            return method;
        }();

        auto* consoleClassMembers = new ClassMembersNode();
        consoleClassMembers->Methods.push_back(readIntMethod);
        consoleClassMembers->Methods.push_back(writeIntMethod);
        consoleClassMembers->Methods.push_back(writeStrMethod);
        consoleClassMembers->Methods.push_back(writeBoolMethod);

        auto* consoleClass = new ClassDeclNode("Console", nullptr, consoleClassMembers);

        readIntMethod->Class = consoleClass;
        writeIntMethod->Class = consoleClass;
        writeBoolMethod->Class = consoleClass;
        writeStrMethod->Class = consoleClass;

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
            Errors.insert(Errors.end(), analyzer.Errors.begin(), analyzer.Errors.end());
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
