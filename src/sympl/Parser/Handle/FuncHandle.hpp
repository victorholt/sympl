//
// GameSencha, LLC 6/1/22.
//
#pragma once
#include <sympl/SymplPCH.hpp>
#include <sympl/Core/StringBuffer.hpp>
#include "ValueHandle.hpp"

SymplNamespaceStart

class SYMPL_API FuncHandle : public ValueHandle
{
    SYMPL_OBJECT(FuncHandle, ValueHandle)

public:
    // Name of the function.
    SharedPtr<StringBuffer> Name;
    // Body of the function.
    SharedPtr<class ParserNode> BodyNode;
    // Argument name list.
    std::vector<std::string> ArgNameList;

    /**
     * Overrides the construct.
     * @param argc
     * @param ArgList
     */
    void __Construct(int argc, va_list ArgList) override;

    /**
     * Creates the handle.
     */
    void Create(
        CStrPtr FuncName,
        const SharedPtr<class ParserNode>& pBodyNode,
        const std::vector<std::string>& pArgNameList
    );

    /**
     * Executes the handle.
     * @param ArgValueList
     * @return
     */
    virtual SharedPtr<class ParserRuntimeResult> Exec(const std::vector<SharedPtr<ValueHandle>>& ArgValueList);

    [[nodiscard]]
    SharedPtr<ValueHandle> Copy() const override;

    /**
     * Returns a string of the value.
     * @return
     */
    CStrPtr ToString() override;
};

SymplNamespaceEnd
