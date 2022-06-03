//
// GameSencha, LLC 6/3/22.
//
#include "SymplVM.hpp"
#include <sympl/Sympl.hpp>
SymplNamespace

std::tuple<SharedPtr<class ValueHandle>, SharedPtr<class ParserError>>
SymplVM::RunScript(CStrPtr FileName, CStrPtr Script)
{
    auto globalSymbolTable = SymbolTable::Alloc<SymbolTable>();

    auto context = SharedPtr<ParserContext>(new ParserContext());
    context->Create(nullptr, nullptr, FileName);
    context->VariableSymbolTable = globalSymbolTable;

    // Global variables.
    globalSymbolTable->Set("null", ValueHandle::Null(context.Ptr()));
    globalSymbolTable->Set("true", ValueHandle::True(context.Ptr()));
    globalSymbolTable->Set("false", ValueHandle::False(context.Ptr()));

    // System functions.
    globalSymbolTable->Set(PrintFunc, BuiltInFuncHandle::Alloc<BuiltInFuncHandle>(1, PrintFunc).Ptr());
    globalSymbolTable->Set(PrintRetFunc, BuiltInFuncHandle::Alloc<BuiltInFuncHandle>(1, PrintRetFunc).Ptr());
    globalSymbolTable->Set(InputFunc, BuiltInFuncHandle::Alloc<BuiltInFuncHandle>(1, InputFunc).Ptr());
    globalSymbolTable->Set(InputIntFunc, BuiltInFuncHandle::Alloc<BuiltInFuncHandle>(1, InputIntFunc).Ptr());
    globalSymbolTable->Set(IsNumberFunc, BuiltInFuncHandle::Alloc<BuiltInFuncHandle>(1, IsNumberFunc).Ptr());
    globalSymbolTable->Set(IsStringFunc, BuiltInFuncHandle::Alloc<BuiltInFuncHandle>(1, IsStringFunc).Ptr());
    globalSymbolTable->Set(IsListFunc, BuiltInFuncHandle::Alloc<BuiltInFuncHandle>(1, IsListFunc).Ptr());
    globalSymbolTable->Set(IsFunctionFunc, BuiltInFuncHandle::Alloc<BuiltInFuncHandle>(1, IsFunctionFunc).Ptr());
    globalSymbolTable->Set(AppendFunc, BuiltInFuncHandle::Alloc<BuiltInFuncHandle>(1, AppendFunc).Ptr());
    globalSymbolTable->Set(RemoveFunc, BuiltInFuncHandle::Alloc<BuiltInFuncHandle>(1, RemoveFunc).Ptr());
    globalSymbolTable->Set(ExtendFunc, BuiltInFuncHandle::Alloc<BuiltInFuncHandle>(1, ExtendFunc).Ptr());
    globalSymbolTable->Set(LengthFunc, BuiltInFuncHandle::Alloc<BuiltInFuncHandle>(1, LengthFunc).Ptr());

    // Utility functions.
    globalSymbolTable->Set(IncludeFunc, BuiltInFuncHandle::Alloc<BuiltInFuncHandle>(1, IncludeFunc).Ptr());

    Lexer lexer(FileName, Script);

    lexer.MakeTokens();
    auto tokens = lexer.GetTokens();
    auto errors = lexer.GetErrors();

    if (!errors.empty()) {
        return std::make_tuple(
            ValueHandle::Null(),
            errors[0]
        );
    }

    Parser parser(tokens);
    auto node = parser.Parse();

    if (node->Error.IsValid()) {
        return std::make_tuple(
            ValueHandle::Null(),
            node->Error
        );
    }

    // Run the program.
    auto program = SharedPtr<Interpreter>(new Interpreter());
    auto result = program->Visit(node->ParserNodePtr, context);

    if (result->Error.IsValid())
    {
        return std::make_tuple(
            ValueHandle::Null(),
            result->Error
        );
    }

    return std::make_tuple(
        result->Value,
        nullptr
    );
}
