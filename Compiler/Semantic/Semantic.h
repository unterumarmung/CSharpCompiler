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
        for (const auto& _namespace : program->Namespaces->GetSeq()) { AnalyzeNamespace(_namespace); }
        if (AllMains.size() > 1) { Errors.push_back("There must be only one main in the program"); }
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

    void Generate()
    {
        if (Errors.empty())
        {
            for (auto* namespace_ : program->Namespaces->GetSeq())
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
};
