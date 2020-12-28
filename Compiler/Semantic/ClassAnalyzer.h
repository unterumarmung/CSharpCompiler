#pragma once
#include <string>
#include <cstdint>
#include <vector>
#include "../Tree/Program.h"

using IdT = uint16_t;
using IntT = std::int32_t;
using FloatT = double;
using Bytes = std::vector<unsigned char>;

inline void append(Bytes& bytes, Bytes::value_type value) { bytes.push_back(value); }

inline void append(Bytes& bytes, Bytes const& value) { bytes.insert(bytes.end(), value.begin(), value.end()); }

inline void append(Bytes& bytes, std::string_view value) { bytes.insert(bytes.end(), value.begin(), value.end()); }

const inline DataType JAVA_BASE_OBJECT = []
{
    DataType type{ DataType::TypeT::Complex };
    type.ComplexType = { "java", "lang", "Object" };
    return type;
}();

Bytes ToBytes(uint32_t n);

Bytes ToBytes(uint16_t n);

Bytes ToBytes(IntT n);

struct Constant
{
    enum class TypeT : uint8_t
    {
        Utf8 = 1,
        Integer = 3,
        Float = 4,
        String = 8,
        NameAndType = 12,
        Class = 7,
        MethodRef = 10,
        FieldRef = 9
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

Bytes ToBytes(Constant const& constant);

struct ConstantTable
{
    std::vector<Constant> Constants;
    using ConstantRef = Constant const&;

    IdT FindUtf8(std::string_view utf8);

    IdT FindInt(IntT i);

    IdT FindFloat(FloatT i);

    IdT FindClass(std::string_view className);

    IdT FindNaT(std::string_view name, std::string_view type);

    IdT FindFieldRef(std::string_view className, std::string_view name, std::string_view type);

    IdT FindMethodRef(std::string_view className, std::string_view name, std::string_view type);
};


Bytes ToBytes(const ConstantTable& constants);

enum class AccessFlags : uint16_t
{
    Public = 0x0001,
    Protected = 0x0004,
    Private = 0x0002,
    Static = 0x0008,
    Final = 0x0010,
    Super = 0x0020
};

inline AccessFlags operator|(AccessFlags lhs, AccessFlags rhs)
{
    using Type = std::underlying_type_t<AccessFlags>;
    const auto lhs2 = static_cast<Type>(lhs);
    const auto rhs2 = static_cast<Type>(rhs);
    return static_cast<AccessFlags>(lhs2 | rhs2);
}

inline AccessFlags ToAccessFlags(VisibilityModifier visibility, const bool isStatic = false)
{
    if (isStatic)
        return AccessFlags::Static | ToAccessFlags(visibility);

    switch (visibility)
    {
    case VisibilityModifier::Public:
        return AccessFlags::Public;
    case VisibilityModifier::Protected:
        return AccessFlags::Protected;
    case VisibilityModifier::Private:
        return AccessFlags::Private;
    }

    return {};
}

struct JvmField
{
    IdT NameId;
    IdT TypeId;
    AccessFlags AccessFlags;
};

Bytes ToBytes(JvmField field);

struct JvmMethod
{
    IdT NameId;
    IdT TypeId;
    AccessFlags AccessFlags;
    MethodDeclNode* ActualMethod;
};

struct ClassFile
{
    static constexpr uint32_t MagicConstant = 0xCAFEBABE;
    static constexpr uint16_t MinorVersion = 0;
    static constexpr uint16_t MajorVersion = 52;

    AccessFlags AccessFlags = {};
    uint16_t ThisClass = 0;
    uint16_t SuperClass = 0;
    static constexpr uint16_t InterfaceCount = 0;

    ConstantTable Constants;
    std::vector<JvmField> Fields;
    std::vector<JvmMethod> Methods;
};

Bytes ToBytes(JvmMethod method, ClassFile& classFile);

struct ClassAnalyzer
{
    MethodDeclNode* CurrentMethod = nullptr;
    ClassDeclNode* CurrentClass = nullptr;
    ClassFile File{};
    std::vector<std::string> Errors{};
    NamespaceDeclNode* Namespace{};
    NamespaceDeclSeq* AllNamespaces{};
    std::vector<MethodDeclNode*> AllMains{};

    explicit ClassAnalyzer(ClassDeclNode* node, NamespaceDeclNode* namespace_, NamespaceDeclSeq* allNamespaces);

    void Analyze();

    void AnalyzeMemberSignatures();

    void AnalyzeVarDecl(VarDeclNode* varDecl, bool withInit = true);

    void AnalyzeWhile(WhileNode* while_);

    void AnalyzeDoWhile(DoWhileNode* doWhile);

    void AnalyzeFor(ForNode* for_);

    void AnalyzeForEach(ForEachNode* forEach);

    void AnalyzeIf(IfNode* if_);

    void AnalyzeReturn(StmtNode* node);

    void AnalyzeStmt(StmtNode* stmt);

    void AnalyzeMethod(MethodDeclNode* method);

    void AnalyzeField(FieldDeclNode* field);

    void AnalyzeClass(ClassDeclNode* value);

    [[nodiscard]] ExprNode* AnalyzeExpr(ExprNode* expr);

    void AnalyzeAccessExpr(AccessExpr* expr);

    void AnalyzeSimpleMethodCall(ExprNode* node);

    void AnalyzeDotMethodCall(ExprNode* node);

    void AnalyzeSimpleMethodCall(AccessExpr* node);

    void AnalyzeDotMethodCall(AccessExpr* node);

    void CalculateTypesForExpr(ExprNode* node);

    DataType CalculateTypeForAccessExpr(AccessExpr* access);

    static ExprNode* ReplaceAssignmentsOnArrayElements(ExprNode* node);

    void ValidateTypename(DataType& dataType);

    [[nodiscard]] ClassDeclNode* FindClass(DataType const& dataType) const;

    void FillTables(FieldDeclNode* field);

    void FillTables(MethodDeclNode* method);

    void FillTables();

    void Generate();

    Bytes ToBytes();
};
