#pragma once
#include <vector>
#include "JvmClass.h"
#include "ClassAnalyzer.h"

struct Semantic
{
    // ReSharper disable once CppInconsistentNaming
    Program* program;
    std::vector<Class> Classes;

    explicit Semantic(Program* program) : program{ program }
    {
    }

    MethodDeclNode* CurrentMethod = nullptr;
    ClassDeclNode* CurrentClass = nullptr;

    void Analyze() { for (const auto& _namespace : program->Namespaces->GetSeq()) { AnalyzeNamespace(_namespace); } }

    void AnalyzeNamespace(NamespaceDeclNode* namespace_)
    {
        for (auto* class_ : namespace_->Members->Classes)
        {
            ClassAnalyzer analyzer(class_);
            analyzer.Analyze();

        }

    }   // TODO enums
};
