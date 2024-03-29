//
// GameSencha, LLC 5/24/22.
//
#include <__sympl_old/include/Parser/Error/ParserError.hpp>
#include <__sympl_old/include/Core/StringBuffer.hpp>
#include <fmt/format.h>
SymplNamespace

ParserError::ParserError(CStrPtr Name, CStrPtr Details)
{
    ErrorName = StringBuffer::Alloc<StringBuffer>();
    ErrorDetails = StringBuffer::Alloc<StringBuffer>();
    ErrorMessage = StringBuffer::Alloc<StringBuffer>();

    ErrorName->Append(Name);
    ErrorDetails->Append(Details);
}

CStrPtr ParserError::ToString()
{
    ErrorMessage->Set(fmt::format("<{0}>: {1}", ErrorName->CStr(), ErrorDetails->CStr()).c_str());
	return ErrorMessage->CStr();
}
