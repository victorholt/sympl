//
// GameSencha, LLC 6/3/22.
//
#include "ObjectHandle.hpp"
#include <sympl/Parser/ParserRuntimeResult.hpp>
#include <sympl/Parser/Interpreter.hpp>
#include <sympl/Parser/ParserContext.hpp>
#include <sympl/Parser/SymbolTable.hpp>
#include <sympl/Parser/Node/ParserNode.hpp>
SymplNamespace

void ObjectHandle::__Construct(int argc, va_list ArgList)
{
    CStrPtr pObjectName = va_arg(ArgList, CStrPtr);
    SharedPtr<ParserNode> pBodyNode = va_arg(ArgList, ParserNode*);
    Create(pObjectName, pBodyNode);
}

void ObjectHandle::Create(
    CStrPtr ObjectName,
    const SharedPtr<ParserNode>& pBodyNode
)
{
    Type = ValueType::Object;
    Name = StringBuffer::Alloc<StringBuffer>();
    Name->Set(ObjectName);
}

SharedPtr<ValueHandle> ObjectHandle::Copy() const
{
    auto NewObject = ObjectHandle::Alloc<ObjectHandle>(2, Name->CStr(), BodyNode.Ptr());
    NewObject->SetPosition(StartPosition, EndPosition);
    NewObject->Context = Context;
    return NewObject.Ptr();
}

CStrPtr ObjectHandle::ToString() {
    StringRep->Set(fmt::format("<object: {0}>", Name->CStr()).c_str());
    return StringRep->CStr();
}
