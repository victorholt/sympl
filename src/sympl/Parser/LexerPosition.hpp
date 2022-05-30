//
// GameSencha, LLC 5/24/22.
//
#pragma once
#include "sympl/SymplPCH.hpp"
#include <sympl/Memory/ManagedObject.hpp>

SymplNamespaceStart

class LexerPosition : public ManagedObject
{
private:
    // Index of the position.
    size_t Index;

    // Line number of the position.
    size_t LineNumber;

    // Line colume of the position.
    int64_t LineCol;

    // Name of the file we're in.
    std::string FileName;

    // Text we're current parsing.
    std::string FileText;

public:
    /**
     * Constructor.
     */
    LexerPosition();

    /**
     * Creates the position.
     * @param pIndex
     * @param pLineNumber
     * @param pCol
     * @param pFileName
     * @param pFileText
     */
    void Create(size_t pIndex, size_t pLineNumber, int64_t pCol, CStrPtr pFileName, CStrPtr pFileText);

    /**
     * Advances the lexer position.
     * @param CurrentCharacter
     */
    void Advance(char CurrentCharacter = '\0');

    /**
     * Copies the values of the position.
     * @return
     */
    SharedPtr<LexerPosition> Copy() const;

    /**
     * Handles copying the object.
     * @param rhs
     * @return
     */
//    LexerPosition& operator=(const LexerPosition& rhs);

    /**
     * Returns the index of the lexer position.
     * @return
     */
    inline size_t GetIndex() const { return Index; }

    /**
     * Returns the line number of the lexer position.
     * @return
     */
    inline size_t GetLineNumber() const { return LineNumber; }

    /**
     * Returns the line column of the lexer position.
     * @return
     */
    inline int64_t GetLineCol() const { return LineCol; }

    /**
     * Returns the file name.
     * @return
     */
    inline CStrPtr GetFileName() const { return FileName.c_str(); }

    /**
     * Returns the file text.
     * @return
     */
    inline CStrPtr GetFileText() const { return FileText.c_str(); }
};

SymplNamespaceEnd
