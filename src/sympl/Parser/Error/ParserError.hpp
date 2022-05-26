//
// GameSencha, LLC 5/24/22.
//
#pragma once
#include "sympl/SymplPCH.hpp"
#include "sympl/Memory/ManagedObject.hpp"

SymplNamespaceStart

class ParserError : public ManagedObject
{
protected:

	char ErrorName[64];
	char ErrorDetails[2048];
	char TmpError_String[2112];

public:

	// Default constructor.
	ParserError() = default;

	/**
	 * Constructor.
	 * @param Name
	 * @param Details
	 */
	ParserError(CStrPtr Name, CStrPtr Details);

	/**
	 * Returns string of the error.
	 * @return
	 */
	CStrPtr ToString();
};

SymplNamespaceEnd
