//
// GameEngine Creations 10/2/2018.
//
#pragma once

class SYMPL_API ArrayResolver : public Object
{
SYMPL_OBJECT(ArrayResolver, Object);

protected:
//! Checks if this is a value assignment and returns the assignment address.
//! \param index
//! \return
bool _CheckValueAssignment(StatementResolver* stmtResolver, ScriptObject* varObject, Variant& index);

public:
ArrayResolver() = default;

//! Called in place of the constructor.
void __Construct() override {}

//! Resolves the array.
//! \param stmtResolver
//! \param currentStr
//! \param varObject
//! \param op
//! \return
virtual Variant Resolve(StatementResolver* stmtResolver, StringBuffer* currentStr,
                        ScriptObject* varObject, StatementOperator op);

//! Resolves the value for an array.
//! \param stmtResolver
//! \param currentStr
//! \param varObject
//! \param op
//! \return
virtual Variant ResolveValue(StatementResolver* stmtResolver, StringBuffer* currentStr, ScriptObject* varObject);
};
