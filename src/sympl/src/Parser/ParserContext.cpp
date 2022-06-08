//
// GameSencha, LLC 5/25/22.
//
#include <sympl/include/Parser/ParserContext.hpp>
#include <sympl/include/Core/StringBuffer.hpp>
#include <sympl/include/Parser/SymbolTable.hpp>
#include <sympl/include/Parser/LexerPosition.hpp>
SymplNamespace

void ParserContext::__Construct(int argc, va_list Args)
{
    DisplayName = StringBuffer::Alloc<StringBuffer>();
}
void ParserContext::__Destruct()
{
    DisplayName = nullptr;
    VariableSymbolTable = nullptr;
    ParentEntryPosition = nullptr;
    Parent = nullptr;
}

void ParserContext::Create(SharedPtr<ParserContext> pParent, SharedPtr<class LexerPosition> pParentEntryPosition,
                           CStrPtr pDisplayName)
{
    Parent = pParent;
    ParentEntryPosition = pParentEntryPosition;

    DisplayName->Clear();
    DisplayName->Append(pDisplayName);
}
