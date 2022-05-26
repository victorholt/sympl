//
// Created by Victor on 5/24/2022.
//
#include "LexerPosition.hpp"
SymplNamespace

void LexerPosition::Create(size_t pIndex, size_t pLineNumber, int64_t pCol, CStrPtr pFileName, CStrPtr pFileText)
{
    Index = pIndex;
    LineNumber = pLineNumber;
    FileName = pFileName;
    FileText = pFileText;
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

SharedPtr<LexerPosition> LexerPosition::Copy() const
{
    auto CopyPosition = LexerPosition::Alloc<LexerPosition>();
    CopyPosition->Create(Index, LineNumber, LineCol, FileName.c_str(), FileText.c_str());

    return CopyPosition;
}

//LexerPosition& LexerPosition::operator=(const LexerPosition& rhs)
//{
//    Index = rhs.Index;
//    LineNumber = rhs.LineNumber;
//    LineCol = rhs.LineCol;
//
//    return *this;
//}
