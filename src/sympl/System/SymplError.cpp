//
// GameSencha, LLC 5/24/22.
//
#include "SymplError.hpp"
SymplNamespace

SymplError::SymplError(CStrPtr Name, CStrPtr Details)
{
	memset(ErrorName, 0, strlen(ErrorName));
	memset(ErrorDetails, 0, strlen(ErrorDetails));

	strcpy(ErrorName, Name);
	strcpy(ErrorDetails, Details);
}

CStrPtr SymplError::ToString()
{
	memset(TmpError_String, 0, strlen(TmpError_String));
	strcpy(TmpError_String, fmt::format("<{0}>: {1}", ErrorName, ErrorDetails).c_str());

	return TmpError_String;
}