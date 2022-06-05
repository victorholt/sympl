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

#include <sympl/include/Memory/ManagedObject.hpp>
#include <sympl/include/Memory/MemBlock.hpp>
#include <sympl/include/Memory/MemPool.hpp>
#include <sympl/include/Memory/SharedPtr.hpp>
#include <sympl/include/Memory/ObjectRef.hpp>

#include "SymplPCH.hpp"
#include <sympl/include/Core/StringBuffer.hpp>

#include <sympl/include/Parser/Error/IllegalCharacterError.hpp>
#include <sympl/include/Parser/Error/InvalidSyntaxError.hpp>
#include <sympl/include/Parser/Error/RuntimeError.hpp>

#include <sympl/include/Parser/Handle/ValueHandle.hpp>
#include <sympl/include/Parser/Handle/NumberHandle.hpp>
#include <sympl/include/Parser/Handle/CompareHandle.hpp>
#include <sympl/include/Parser/Handle/IntHandle.hpp>
#include <sympl/include/Parser/Handle/FloatHandle.hpp>
#include <sympl/include/Parser/Handle/ListHandle.hpp>
#include <sympl/include/Parser/Handle/StringHandle.hpp>
#include <sympl/include/Parser/Handle/BaseFuncHandle.hpp>
#include <sympl/include/Parser/Handle/BuiltInFuncHandle.hpp>
#include <sympl/include/Parser/Handle/FuncHandle.hpp>
#include <sympl/include/Parser/Handle/ObjectHandle.hpp>
#include <sympl/include/Parser/Handle/ExceptionHandle.hpp>
#include <sympl/include/Parser/Handle/NullHandle.hpp>

#include <sympl/include/Parser/Node/ParserBinaryOpNode.hpp>
#include <sympl/include/Parser/Node/ParserCallNode.hpp>
#include <sympl/include/Parser/Node/ParserForNode.hpp>
#include <sympl/include/Parser/Node/ParserFuncDefNode.hpp>
#include <sympl/include/Parser/Node/ParserObjectNode.hpp>
#include <sympl/include/Parser/Node/ParserNewObjectNode.hpp>
#include <sympl/include/Parser/Node/ParserScopeAccessNode.hpp>
#include <sympl/include/Parser/Node/ParserIfNode.hpp>
#include <sympl/include/Parser/Node/ParserListNode.hpp>
#include <sympl/include/Parser/Node/ParserNumberNode.hpp>
#include <sympl/include/Parser/Node/ParserStringNode.hpp>
#include <sympl/include/Parser/Node/ParserUnaryOpNode.hpp>
#include <sympl/include/Parser/Node/ParserWhileNode.hpp>
#include <sympl/include/Parser/Node/ParserBreakNode.hpp>
#include <sympl/include/Parser/Node/ParserReturnNode.hpp>
#include <sympl/include/Parser/Node/ParserContinueNode.hpp>
#include <sympl/include/Parser/Node/VarAccessNode.hpp>
#include <sympl/include/Parser/Node/VarAssignNode.hpp>

#include <sympl/include/Parser/Token.hpp>
#include <sympl/include/Parser/LexerPosition.hpp>
#include <sympl/include/Parser/Lexer.hpp>
#include <sympl/include/Parser/SymbolTable.hpp>
#include <sympl/include/Parser/Parser.hpp>
#include <sympl/include/Parser/ParseResult.hpp>
#include <sympl/include/Parser/ParserRuntimeResult.hpp>
#include <sympl/include/Parser/ParserContext.hpp>
#include <sympl/include/Parser/Interpreter.hpp>
#include <sympl/include/Parser/SymplVM.hpp>
