/**********************************************************
 * Author:  GameSencha, LLC
 * The MIT License (MIT)
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a
 *  copy of this software and associated documentation files (the "Software"),
 *  to deal in the Software without restriction, including without limitation
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *  and/or sell copies of the Software, and to permit persons to whom the
 *  Software is furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 *  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 *  DEALINGS IN THE SOFTWARE.
 *
 **********************************************************/
#pragma once

#include <sympl/Memory/ManagedObject.hpp>
#include <sympl/Memory/MemBlock.hpp>
#include <sympl/Memory/MemPool.hpp>
#include <sympl/Memory/SharedPtr.hpp>
#include <sympl/Memory/ObjectRef.hpp>

#include "SymplPCH.hpp"
#include <sympl/Core/StringBuffer.hpp>
#include <sympl/Parser/Error/IllegalCharacterError.hpp>
#include <sympl/Parser/Error/InvalidSyntaxError.hpp>
#include <sympl/Parser/Error/InvalidSyntaxError.hpp>
#include <sympl/Parser/Error/RuntimeError.hpp>
#include <sympl/Parser/Token.hpp>
#include <sympl/Parser/LexerPosition.hpp>
#include <sympl/Parser/Lexer.hpp>
#include <sympl/Parser/SymbolTable.hpp>
#include <sympl/Parser/Handle/ValueHandle.hpp>
#include <sympl/Parser/Handle/NumberHandle.hpp>
#include <sympl/Parser/Handle/IntHandle.hpp>
#include <sympl/Parser/Handle/FloatHandle.hpp>
#include <sympl/Parser/Node/ParserBinaryOpNode.hpp>
#include <sympl/Parser/Node/ParserUnaryOpNode.hpp>
#include <sympl/Parser/Node/ParserUnaryOpNode.hpp>
#include <sympl/Parser/Node/VarAccessNode.hpp>
#include <sympl/Parser/Node/VarAssignNode.hpp>
#include <sympl/Parser/Parser.hpp>
#include <sympl/Parser/ParserRuntimeResult.hpp>
#include <sympl/Parser/ParserContext.hpp>
#include <sympl/Parser/Interpreter.hpp>
