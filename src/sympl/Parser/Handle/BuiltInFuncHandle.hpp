//
// GameSencha, LLC 6/1/22.
//
#pragma once
#include <sympl/SymplPCH.hpp>
#include <sympl/Core/StringBuffer.hpp>
#include "BaseFuncHandle.hpp"

SymplNamespaceStart

typedef std::function<SharedPtr<ParserRuntimeResult>(SharedPtr<ParserContext> pExecContext)> BuiltInMethodType;

class SYMPL_API BuiltInFuncHandle : public BaseFuncHandle
{
	SYMPL_OBJECT(BuiltInFuncHandle, BaseFuncHandle)

protected:
	// Method mapping.
	std::unordered_map<std::string, BuiltInMethodType> MethodMap;
	// Method argument mapping.
	std::unordered_map<std::string, std::vector<std::string>> MethodArgMap;

public:

	/**
	 * Overrides the construct.
	 * @param argc
	 * @param ArgList
	 */
	void __Construct(int argc, va_list ArgList) override;

	/**
	 * Creates the handle.
	 */
	void Create(CStrPtr pFuncName);

	/**
	 * Executes the handle.
	 * @param ArgValueList
	 * @return
	 */
	virtual SharedPtr<class ParserRuntimeResult> Exec(const std::vector<SharedPtr<ValueHandle>>& pArgValueList);

	/**
	 * Executes the print method.
	 * @param pExecContext
	 * @return
	 */
	SharedPtr<ParserRuntimeResult> ExecPrint(SharedPtr<class ParserContext>& pExecContext);

	/**
	 * Executes the print return method.
	 * @param pExecContext
	 * @return
	 */
	SharedPtr<ParserRuntimeResult> ExecPrintRet(SharedPtr<class ParserContext>& pExecContext);

	/**
	 * Executes the input method.
	 * @param pExecContext
	 * @return
	 */
	SharedPtr<ParserRuntimeResult> ExecInput(SharedPtr<class ParserContext>& pExecContext);

	/**
	 * Executes the input (int-only) method.
	 * @param pExecContext
	 * @return
	 */
	SharedPtr<ParserRuntimeResult> ExecInputInt(SharedPtr<class ParserContext>& pExecContext);

	/**
	 * Executes the IsType method.
	 * @param pExecContext
	 * @return
	 */
	SharedPtr<ParserRuntimeResult> ExecIsType(
		SharedPtr<struct ParserContext> &pExecContext,
		const std::vector<ValueType>& pTypes
	);

	/**
	 * Executes the Append method.
	 * @param pExecContext
	 * @return
	 */
	SharedPtr<ParserRuntimeResult> ExecAppend(SharedPtr<class ParserContext>& pExecContext);

	/**
	 * Executes the Remove method.
	 * @param pExecContext
	 * @return
	 */
	SharedPtr<ParserRuntimeResult> ExecRemove(SharedPtr<class ParserContext>& pExecContext);

	/**
	 * Executes the Extend method.
	 * @param pExecContext
	 * @return
	 */
	SharedPtr<ParserRuntimeResult> ExecExtend(SharedPtr<class ParserContext>& pExecContext);

    /**
	 * Executes the length method.
	 * @param pExecContext
	 * @return
	 */
    SharedPtr<ParserRuntimeResult> ExecLength(SharedPtr<class ParserContext>& pExecContext);

    /**
	 * Executes the str method.
	 * @param pExecContext
	 * @return
	 */
    SharedPtr<ParserRuntimeResult> ExecStr(SharedPtr<class ParserContext>& pExecContext);

    /**
	 * Executes the time method.
	 * @param pExecContext
	 * @return
	 */
    SharedPtr<ParserRuntimeResult> ExecTime(SharedPtr<class ParserContext>& pExecContext);

    /**
	 * Executes the export method.
	 * @param pExecContext
	 * @return
	 */
    SharedPtr<ParserRuntimeResult> ExecExport(SharedPtr<class ParserContext>& pExecContext);

    /**
	 * Executes the include method.
	 * @param pExecContext
	 * @return
	 */
    SharedPtr<ParserRuntimeResult> ExecInclude(SharedPtr<class ParserContext>& pExecContext);

    /**
	 * Executes the include method.
	 * @param pExecContext
	 * @return
	 */
    SharedPtr<ParserRuntimeResult> ExecRun(SharedPtr<class ParserContext>& pExecContext);

	/**
	 * Method where there was no execute.
	 * @param pExecContext
	 * @return
	 */
	SharedPtr<ParserRuntimeResult> NoVisitMethod(const SharedPtr<class ParserContext>& pExecContext);

	/**
	 * Method had an invalid argument.
	 * @param pExecContext
	 * @return
	 */
	SharedPtr<ParserRuntimeResult> InvalidMethodArgument(const SharedPtr<class ParserContext>& pExecContext, CStrPtr ArgName, CStrPtr ExpectedStr);

	/**
	 * Adds a method.
	 * @param MethodName
	 * @param MethodHandle
	 * @param MethodArgNameList
	 */
	void AddMethod(CStrPtr MethodName, const BuiltInMethodType& MethodHandle, const std::vector<std::string>& MethodArgNameList);

	/**
     * Copies the value.
     * @return
     */
	[[nodiscard]]
	SharedPtr<ValueHandle> Copy() const override;

	/**
	 * Returns a string of the value.
	 * @return
	 */
	CStrPtr ToString() override;
};

SymplNamespaceEnd
