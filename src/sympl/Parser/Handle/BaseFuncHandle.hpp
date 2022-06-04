//
// GameSencha, LLC 6/2/22.
//
#pragma once
#include <sympl/SymplPCH.hpp>
#include <sympl/Core/StringBuffer.hpp>
#include "ValueHandle.hpp"

SymplNamespaceStart

class SYMPL_API BaseFuncHandle : public ValueHandle
{
	SYMPL_OBJECT(BaseFuncHandle, ValueHandle)

public:
	// Name of the function.
	SharedPtr<StringBuffer> Name;
    // Check if we should return the expression of the function.
    bool ShouldAutoReturn = true;

	/**
	 * Overrides the construct.
	 * @param argc
	 * @param ArgList
	 */
	void __Construct(int argc, va_list ArgList) override;

	/**
	 * Creates the handle.
	 */
	void Create(CStrPtr FuncName);

	/**
	 * Checks to see if we have a valid set of arguments.
	 * @param pArgNameList
	 * @param pArgValueList
	 * @return
	 */
	SharedPtr<class ParserRuntimeResult> CheckArgs(const std::vector<std::string>& pArgNameList, const std::vector<SharedPtr<ValueHandle>>& pArgValueList);

	/**
	 * Populates the arguments.
	 * @param pArgValueList
	 */
	void PopulateArgs(
		const std::vector<std::string>& pArgNameList,
		const std::vector<SharedPtr<ValueHandle>>& pArgValueList,
		SharedPtr<class ParserContext> ExecContext
	);

	/**
	 * Checks and populates the arguments.
	 * @param pArgNameList
	 * @param pArgValueList
	 * @return
	 */
	SharedPtr<class ParserRuntimeResult> CheckAndPopulateArgs(
		const std::vector<std::string>& pArgNameList,
		const std::vector<SharedPtr<ValueHandle>>& pArgValueList,
		SharedPtr<class ParserContext> ExecContext
	);

	/**
	 * Executes the handle.
	 * @param ArgValueList
	 * @return
	 */
	virtual SharedPtr<class ParserRuntimeResult> Exec(const std::vector<SharedPtr<ValueHandle>>& ArgValueList);

	/**
	 * Returns a string of the value.
	 * @return
	 */
	CStrPtr ToString() override;
};

SymplNamespaceEnd
