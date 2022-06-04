//
// GameSencha, LLC 6/3/22.
//
#pragma once
#include <sympl/SymplPCH.hpp>
#include <sympl/Core/StringBuffer.hpp>
#include "ValueHandle.hpp"

SymplNamespaceStart

class SYMPL_API ObjectHandle : public ValueHandle
{
    SYMPL_OBJECT(ObjectHandle, ValueHandle)

public:
    // Name of the object.
    SharedPtr<StringBuffer> Name;

    // Body of the function.
    SharedPtr<class ParserNode> BodyNode;

    // Check if this is an instanced object.
    bool IsInstancedObject = false;

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
        CStrPtr ObjectName,
        const SharedPtr<class ParserNode>& pBodyNode
    );

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
