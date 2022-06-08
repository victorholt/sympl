//
// GameSencha, LLC 5/24/22.
//
#pragma once
#include <sympl/SymplPCH.hpp>
#include <sympl/include/Memory/ManagedObject.hpp>

SymplNamespaceStart

class SYMPL_API ParserNode : public ManagedObject
{
	SYMPL_OBJECT(ParserNode, ManagedObject)

protected:
    // Handles conversion of the node to a string.
    char TmpNodeString_Allocate[255];

    // Reference to the token associated with the node.
    SharedPtr<class Token> NodeToken;

    // Start position of the token.
    SharedPtr<class LexerPosition> StartPosition;

    // End position of the token.
    SharedPtr<class LexerPosition> EndPosition;

public:
    // Type of node.
    ParseNodeType Type;

    // Check if we should return null.
    bool ShouldReturnNull = false;
    // For methods, should auto-return.
    bool ShouldAutoReturn = false;

    /**
     * Overrides the construct.
     * @param argc
     * @param ArgList
     */
    void __Construct(int argc, va_list ArgList) override;

    /**
     * Destructor.
     */
    void __Destruct() override;

    /**
     * Creates the parser node.
     * @param pType
     * @param pNodeToken
     */
    void Create(ParseNodeType pType, const SharedPtr<class Token>& pNodeToken);

    /**
     * Returns the node token.
     * @return
     */
    SharedPtr<class Token> GetNodeToken() const;

    /**
     * Sets the node token.
     * @param pNodeToken
     */
    void SetNodeToken(const SharedPtr<class Token>& pNodeToken);

    /**
     * Returns the start position.
     * @return
     */
    SharedPtr<class LexerPosition> GetStartPosition() const;

    /**
     * Returns the end position.
     * @return
     */
    SharedPtr<class LexerPosition> GetEndPosition() const;

    /**
     * Sets the positions.
     * @param pStartPosition
     * @param pEndPosition
     */
    void SetPosition(const SharedPtr<class LexerPosition>& pStartPosition,
                    const SharedPtr<class LexerPosition>& pEndPosition);

    /**
     * Returns the string representation of the node.
     * @return
     */
    virtual CStrPtr ToString();
};

SymplNamespaceEnd
