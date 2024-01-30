//
// GameSencha, LLC 5/24/22.
//
#pragma once
#include <__sympl_old/SymplPCH.hpp>
#include <__sympl_old/include/Memory/ManagedObject.hpp>

SymplNamespaceStart

class SYMPL_API ParserError : public ManagedObject
{
	SYMPL_OBJECT(ParserError, ManagedObject)

protected:
    // Error name string.
    SharedPtr<class StringBuffer> ErrorName;
    // Error details string.
    SharedPtr<class StringBuffer> ErrorDetails;
    // Error message string.
    SharedPtr<class StringBuffer> ErrorMessage;

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
	virtual CStrPtr ToString();
};

SymplNamespaceEnd
