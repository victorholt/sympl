//
// Created by Victor on 5/24/2022.
//
#include "LexerPosition.hpp"
SymplNamespace

LexerPosition::LexerPosition(size_t pIndex, size_t pLineNumber, int64_t pCol, CStrPtr pFileName, CStrPtr pFileText)
    : Index(pIndex), LineNumber(pLineNumber), LineCol(pCol),
    FileName(pFileName), FileText(pFileText)
{

}

void LexerPosition::Advance(char CurrentCharacter)
{
    Index++;
    LineCol++;

    if (CurrentCharacter == '\n')
    {
        LineNumber++;
        LineCol = 0;
    }
}

LexerPosition* LexerPosition::Copy() const
{
    return new LexerPosition(Index, LineNumber, LineCol, FileName.c_str(), FileText.c_str());
}

LexerPosition& LexerPosition::operator=(const LexerPosition& rhs)
{
    Index = rhs.Index;
    LineNumber = rhs.LineNumber;
    LineCol = rhs.LineCol;

    return *this;
}
