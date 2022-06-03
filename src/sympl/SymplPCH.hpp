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

// CPP Core Files
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#pragma warning( disable : 4251 )
#pragma warning( disable : 4715 )
#pragma warning( disable : 4244 )
#pragma warning( disable : 4267 )

#ifdef _WIN32_DISABLE_CONSOLE
// https://stackoverflow.com/questions/2139637/hide-console-of-windows-application
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif
#endif

#include <string>
#include <iostream>
#include <sstream>

#include <algorithm>
#include <atomic>
#include <array>

#include <ctime>
#include <cstring>
#include <ctype.h>
#include <cctype>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>

#include <fcntl.h>
#include <fstream>
#include <functional>
#include <future>

#include <locale>
#include <list>

#include <istream>
#include <ostream>
#include <streambuf>
#include <iterator>

#include <math.h>
#include <memory>
#include <mutex>

#include <numeric>

#include <regex>

#include <set>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <typeinfo>
#include <type_traits>
#include <thread>

#include <unordered_set>
#include <unordered_map>
#include <utility>

#include <vector>

#include <wchar.h>

#include <tuple>

#ifdef _WIN32
#include <uchar.h>
#endif

#ifndef _WIN32
#include <sys/time.h>
#define w_gettimeofday gettimeofday

#include <dirent.h>
#include <getopt.h>
#include <unistd.h>

#define IsSpaceChar(c, loc) std::isspace(c, loc)

#else
#include <time.h>
#include "sympl/thirdparty/direntutil.h"
#include "sympl/thirdparty/getoptutil.h"
#include "sympl/thirdparty/timeutil.h"
#include "sympl/thirdparty/unistdutil.h"
#include "sympl/thirdparty/dynamic_mem_pool.h"

//#define IsSpaceChar(c, loc) std::isspace(static_cast<int>(c), loc)
#define IsSpaceChar(c, loc) std::isspace(c, loc)
#endif

#ifdef _WIN32
#define GUID_WINDOWS
#elif defined(_APPLE)
#define GUID_CFUUID
#elif defined(__ANDROID__)
#define GUID_ANDROID
#else
    #define GUID_LIBUUID
#endif

//#include <sympl/thirdparty/guid.h>

#ifdef _WIN32
#ifndef SYMPL_IMPORTS
/* We are building this library */
#define SYMPL_API __declspec(dllexport)
#else
/* We are using this library */
    #define SYMPL_API __declspec(dllimport)
#endif
#else
#define SYMPL_API
#endif

#define IsNullObject(Object) (Object == NULL || Object == nullptr)

#define SymplNamespaceStart namespace Sympl {
#define SymplNamespaceEnd }
#define SymplNamespace using namespace Sympl;

#ifdef SYMPL_DEBUG
#include <assert.h>
#define sympl_assert(stmt) assert(stmt)
#else
#define sympl_assert(stmt)
#endif

#define SYMPL_VERSION 0x00010000 // ver. 0.1.0.0

#include "sympl/thirdparty/fmt/format.h"

// Helpers
#define alloc_bytes(type) new type()
#define alloc_bytes(type) new type()
#define alloc_bytes_array(type, amount) (type*)calloc(amount, sizeof(type))
#define free_bytes(ref) delete ref; ref = nullptr
#define free_bytes_array(ref) free(ref); ref = nullptr
#define TokenValue(x) TokenTypeValueList[(int)x]
#define TokenValueChar(x) TokenTypeValueList[(int)x][0]
#define TokenIsKeyword(tok, key) (tok->GetType() == TokenType::Keyword && (strcmp(tok->GetValue(), key) == 0))
#define TokenIsNotKeyword(tok, key) (tok->GetType() != TokenType::Keyword || (strcmp(tok->GetValue(), key) != 0))
#define StringEquals(a, b) (strcmp(a, b) == 0)
#define StringNotEquals(a, b) (strcmp(a, b) != 0)

#define VectorHas(vec, key) (std::find(vec.begin(), vec.end(), key) != vec.end())

// Definitions
typedef const char* CStrPtr;
typedef char* StrPtr;

// Base token types.
SymplNamespaceStart

enum class TokenType
{
    EndOfFile,
    Int, Float, String,
    Plus, Minus, Mul, Div, Power,
    LH_Parenth, RH_Parenth,
    L_SqrBracket, R_SqrBracket,
    Identifier, Keyword,
    Equals,
    IsEqual, NotEqual, LessThan, LessThanOrEqual, GreaterThan, GreaterThanOrEqual,
    Not, Or, And,
    Comma,
    Arrow,
    NewLine, CloseStatement
};
static const std::vector<CStrPtr> TokenTypeValueList = {
	"EOF", "int", "float", "\"",
	"+", "-", "*", "/", "^", "(", ")", "[", "]",
	"identifier", "keyword",
	"=", "==", "!=", "<", "<=", ">", ">=", "!", "||", "&&",
    ",", "->", "\n", ";"
};

enum class ParseNodeType
{
    Null = 0,
    Number,
    Binary,
    Unary,
    VarAccess,
    VarAssign,
	If,
	For,
	While,
    Func,
    Call,
    String,
    List,
    Return,
    Break,
    Continue
};

enum class ValueType
{
    Null = 0,
    Int,
    Float,
	String,
    Func,
    List,
	Exception
};

// Valid keywords.
static const char* VarKeyword = "var";
static const char* IfKeyword = "if";
static const char* ThenKeyword = "then";
static const char* ElseIfKeyword = "elif";
static const char* ElseKeyword = "else";
static const char* ForKeyword = "for";
static const char* ToKeyword = "to";
static const char* StepKeyword = "step";
static const char* WhileKeyword = "while";
static const char* FuncKeyword = "func";
static const char* EndKeyword = "end";
static const char* ReturnKeyword = "return";
static const char* BreakKeyword = "break";
static const char* ContinueKeyword = "continue";

static const std::vector<const char*> BuiltInKeywordList = {
    VarKeyword,
    "&&", "||", "!",
	IfKeyword, ThenKeyword, ElseIfKeyword, ElseKeyword,
	ForKeyword, ToKeyword, StepKeyword, WhileKeyword,
    FuncKeyword,
    EndKeyword,
    ReturnKeyword, BreakKeyword, ContinueKeyword
};

// Built-In Functions.
static const char* PrintFunc = "print";
static const char* PrintRetFunc = "printRet";
static const char* InputFunc = "input";
static const char* InputIntFunc = "inputInt";
static const char* IsNumberFunc = "isNumber";
static const char* IsStringFunc = "isString";
static const char* IsListFunc = "isList";
static const char* IsFunctionFunc = "isFunction";
static const char* AppendFunc = "append";
static const char* RemoveFunc = "remove";
static const char* ExtendFunc = "extend";
static const char* IncludeFunc = "include";
static const char* LengthFunc = "length";

SymplNamespaceEnd
