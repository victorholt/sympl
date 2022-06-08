//
// GameSencha, LLC 6/3/22.
//
#include <sympl/include/Parser/Handle/ObjectHandle.hpp>
#include <sympl/include/Parser/ParserRuntimeResult.hpp>
#include <sympl/include/Parser/Interpreter.hpp>
#include <sympl/include/Parser/ParserContext.hpp>
#include <sympl/include/Parser/SymbolTable.hpp>
#include <sympl/include/Parser/Token.hpp>
#include <sympl/include/Parser/Node/ParserNode.hpp>
#include <sympl/include/Parser/Node/ParserListNode.hpp>
SymplNamespace

void ObjectHandle::__Construct(int argc, va_list ArgList)
{
    ValueHandle::__Construct(argc, ArgList);

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
    BodyNode = pBodyNode;
}

SharedPtr<ValueHandle> ObjectHandle::Copy() const
{
    auto NewObject = ObjectHandle::Alloc<ObjectHandle>(2, Name->CStr(), BodyNode.Ptr());
    NewObject->SetPosition(StartPosition, EndPosition);
    NewObject->IsInstancedObject = IsInstancedObject;
    NewObject->Context = Context;

    auto NewBodyNode = ParserListNode::Alloc<ParserListNode>();
    NewBodyNode->SetNodeToken(BodyNode->GetNodeToken());
    NewBodyNode->SetPosition(BodyNode->GetStartPosition(), BodyNode->GetEndPosition());

    SharedPtr<ParserListNode> BodyListValues = ObjectRef::CastTo<ParserListNode>(BodyNode.Ptr());
    if (BodyListValues.IsValid()) {
        for (auto& Item: BodyListValues->ElementNodeList) {
            NewBodyNode->ElementNodeList.emplace_back(Item);
        }
    }

    NewObject->BodyNode = NewBodyNode.Ptr();
    return NewObject.Ptr();
}

CStrPtr ObjectHandle::ToString() {
    StringRep->Set(fmt::format("<object>: {0}", Name->CStr()).c_str());

    // Build the string for the object values.
    if (!IsInstancedObject) {
        return StringRep->CStr();
    }

    StringRep->Append(" { ");
    SharedPtr<ParserListNode> BodyListValues = ObjectRef::CastTo<ParserListNode>(BodyNode.Ptr());
    if (BodyListValues.IsValid()) {
        size_t StringIndex = 0;
        for (auto& Item: BodyListValues->ElementNodeList) {
            auto ItemValue = Context->VariableSymbolTable->Get(Item->GetNodeToken()->GetValue());
            CStrPtr ValueStr = ItemValue.IsValid() ? ItemValue->ToString() : "null";

            StringRep->Append(fmt::format("{0}:{1}", Item->GetNodeToken()->GetValue(), ValueStr));
            StringIndex++;

            if (StringIndex < BodyListValues->ElementNodeList.size()) {
                StringRep->Append(", ");
            }
        }
    }
    StringRep->Append(" }");

    return StringRep->CStr();
}
