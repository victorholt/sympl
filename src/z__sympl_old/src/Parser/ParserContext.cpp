//
// GameSencha, LLC 5/25/22.
//
#include <__sympl_old/include/Parser/ParserContext.hpp>
#include <__sympl_old/include/Core/StringBuffer.hpp>
#include <__sympl_old/include/Parser/SymbolTable.hpp>
#include <__sympl_old/include/Parser/LexerPosition.hpp>
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
