//
// GameSencha, LLC 5/25/22.
//
#include "ParserContext.hpp"
#include <sympl/Core/StringBuffer.hpp>
SymplNamespace

ParserContext::ParserContext()
{
    DisplayName = StringBuffer::Alloc<StringBuffer>();
}

void ParserContext::Create(SharedPtr<ParserContext> pParent, SharedPtr<struct LexerPosition> pParentEntryPosition,
                           CStrPtr pDisplayName)
{
    Parent = pParent;
    ParentEntryPosition = pParentEntryPosition;

    DisplayName->Clear();
    DisplayName->Append(pDisplayName);
}
