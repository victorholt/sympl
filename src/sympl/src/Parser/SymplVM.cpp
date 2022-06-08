//
// GameSencha, LLC 6/3/22.
//
#include <sympl/include/Parser/SymplVM.hpp>
#include <sympl/include/Parser/ParserContext.hpp>
#include <sympl/include/Parser/SymbolTable.hpp>
#include <sympl/include/Parser/Token.hpp>
#include <sympl/include/Parser/Error/ParserError.hpp>
#include <sympl/Sympl.hpp>
SymplNamespace

void SymplVM::__Construct(int argc, va_list ArgList)
{
    SharedPtr<ParserContext> ExistingContext;
    if (argc > 0) {
        ExistingContext = va_arg(ArgList, ParserContext*);
    }

    // Create a new context/symbol table.
    if (argc == 0 || !ExistingContext.IsValid()) {
        GlobalSymbolTable = SymbolTable::Alloc<SymbolTable>();

        GlobalContext = ParserContext::Alloc<ParserContext>();
        GlobalContext->Create(nullptr, nullptr, "Context");
        GlobalContext->VariableSymbolTable = GlobalSymbolTable;

        // Global variables.
        GlobalSymbolTable->Set("null", ValueHandle::Null(GlobalContext.Ptr()));
        GlobalSymbolTable->Set("true", ValueHandle::True(GlobalContext.Ptr()));
        GlobalSymbolTable->Set("false", ValueHandle::False(GlobalContext.Ptr()));

        // System functions.
        GlobalSymbolTable->Set(PrintFunc, BuiltInFuncHandle::Alloc<BuiltInFuncHandle>(1, PrintFunc).Ptr());
        GlobalSymbolTable->Set(PrintRetFunc, BuiltInFuncHandle::Alloc<BuiltInFuncHandle>(1, PrintRetFunc).Ptr());
        GlobalSymbolTable->Set(InputFunc, BuiltInFuncHandle::Alloc<BuiltInFuncHandle>(1, InputFunc).Ptr());
        GlobalSymbolTable->Set(InputIntFunc, BuiltInFuncHandle::Alloc<BuiltInFuncHandle>(1, InputIntFunc).Ptr());
        GlobalSymbolTable->Set(IsNumberFunc, BuiltInFuncHandle::Alloc<BuiltInFuncHandle>(1, IsNumberFunc).Ptr());
        GlobalSymbolTable->Set(IsStringFunc, BuiltInFuncHandle::Alloc<BuiltInFuncHandle>(1, IsStringFunc).Ptr());
        GlobalSymbolTable->Set(IsListFunc, BuiltInFuncHandle::Alloc<BuiltInFuncHandle>(1, IsListFunc).Ptr());
        GlobalSymbolTable->Set(IsFunctionFunc, BuiltInFuncHandle::Alloc<BuiltInFuncHandle>(1, IsFunctionFunc).Ptr());
        GlobalSymbolTable->Set(AppendFunc, BuiltInFuncHandle::Alloc<BuiltInFuncHandle>(1, AppendFunc).Ptr());
        GlobalSymbolTable->Set(RemoveFunc, BuiltInFuncHandle::Alloc<BuiltInFuncHandle>(1, RemoveFunc).Ptr());
        GlobalSymbolTable->Set(ExtendFunc, BuiltInFuncHandle::Alloc<BuiltInFuncHandle>(1, ExtendFunc).Ptr());
        GlobalSymbolTable->Set(LengthFunc, BuiltInFuncHandle::Alloc<BuiltInFuncHandle>(1, LengthFunc).Ptr());
        GlobalSymbolTable->Set(StrFunc, BuiltInFuncHandle::Alloc<BuiltInFuncHandle>(1, StrFunc).Ptr());
        GlobalSymbolTable->Set(TimeFunc, BuiltInFuncHandle::Alloc<BuiltInFuncHandle>(1, TimeFunc).Ptr());

        // Utility functions.
        GlobalSymbolTable->Set(ExportFunc, BuiltInFuncHandle::Alloc<BuiltInFuncHandle>(1, ExportFunc).Ptr());
        GlobalSymbolTable->Set(IncludeFunc, BuiltInFuncHandle::Alloc<BuiltInFuncHandle>(1, IncludeFunc).Ptr());
        GlobalSymbolTable->Set(RunFunc, BuiltInFuncHandle::Alloc<BuiltInFuncHandle>(1, RunFunc).Ptr());
    }
    else {
        GlobalSymbolTable = ExistingContext->VariableSymbolTable;
        GlobalContext = ExistingContext;
    }
}

void SymplVM::__Destruct()
{
    GlobalContext = nullptr;
    GlobalSymbolTable = nullptr;
}

std::tuple<SharedPtr<class ValueHandle>, SharedPtr<class ParserError>>
SymplVM::RunScript(CStrPtr FileName, CStrPtr Script)
{
    std::vector<SharedPtr<Token>> tokens;
    std::vector<SharedPtr<ParserError>> errors;

    auto lexer = Lexer::Alloc<Lexer>();
    lexer->Create(FileName, Script);

    lexer->MakeTokens();

    tokens = lexer->GetTokens();
    errors = lexer->GetErrors();

    if (!errors.empty()) {
        return std::make_tuple(
            ValueHandle::Null(),
            errors[0]
        );
    }

    auto parser = Parser::Alloc<Parser>();
    parser->Create(tokens);
    auto node = parser->Parse();

//    if (IsChildVM) {
//        return std::make_tuple(
//                nullptr,
//                nullptr
//        );
//    }

    if (node->Error.IsValid()) {
        return std::make_tuple(
                ValueHandle::Null(),
                node->Error
        );
    }

    // Run the program.
    auto program = SharedPtr<Interpreter>(new Interpreter());
	auto result = program->Visit(node->ParserNodePtr, GlobalContext);

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

SharedPtr<class ParserContext> SymplVM::GetContext() const
{
    return GlobalContext;
}

SharedPtr<class SymbolTable> SymplVM::GetSymbolTable() const
{
    return GlobalSymbolTable;
}
