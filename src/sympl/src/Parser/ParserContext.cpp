//
// GameSencha, LLC 5/25/22.
//
#include <sympl/include/Parser/ParserContext.hpp>
#include <sympl/include/Core/StringBuffer.hpp>
SymplNamespace

ParserContext::ParserContext()
{
    DisplayName = StringBuffer::Alloc<StringBuffer>();
}

void ParserContext::Create(SharedPtr<ParserContext> pParent, SharedPtr<class LexerPosition> pParentEntryPosition,
                           CStrPtr pDisplayName)
{
    Parent = pParent;
    ParentEntryPosition = pParentEntryPosition;

    DisplayName->Clear();
    DisplayName->Append(pDisplayName);
}
