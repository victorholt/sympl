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
#include "script_compiler.h"
#include "script_reader.h"

#include <fmt/format.h>
sympl_namespaces

ScriptCompiler::ScriptCompiler()
{
    _CurrentObjectBuffer = alloc_ref(StringBuffer);
    _CurrentObjectBuffer->Resize(2000);

    _CurrentValueBuffer = alloc_ref(StringBuffer);
    _CurrentValueBuffer->Resize(2000);

    _ClearBuffers();
}

ScriptCompiler::~ScriptCompiler()
{
    free_ref(StringBuffer, _CurrentObjectBuffer);
    free_ref(StringBuffer, _CurrentValueBuffer);
}

void ScriptCompiler::CompileFile(const char* filePath)
{
    auto reader = alloc_ref(ScriptReader);
    reader->ReadFile(filePath);
    _ParseBuffer(reader);
    free_ref(ScriptReader, reader);
}

void ScriptCompiler::CompileString(const char* str)
{
    auto reader = alloc_ref(ScriptReader);
    reader->ReadString(str);
    _ParseBuffer(reader);
    free_ref(ScriptReader, reader);
}

void ScriptCompiler::_ParseBuffer(ScriptReader* reader)
{
    int nestLevel = 0;
    int currentLine = 1;
    char previousChar = '\0';
    char previousValidChar = '\0';
    bool recording = false;
    bool newWord = false;
    bool isComment = false;
    bool isBlockComment = false;
    bool valueMode = false;
    bool inArray = false;
    std::locale loc;

    // Open the file.
    char currentChar;

    std::ifstream fileStream (reader->GetBuffer()->CStr());
    if (!fileStream.is_open()) {
        return;
    }

    fileStream.seekg(0, fileStream.end);
    size_t fileLength = static_cast<size_t>(fileStream.tellg());
    fileStream.seekg(0, fileStream.beg);

    while (fileStream.get(currentChar)) {
        char nextChar = static_cast<char>(fileStream.peek());
        std::cout << currentChar;
    }
    std::cout << std::endl;
}

void ScriptCompiler::_BuildObject()
{

}

void ScriptCompiler::_ClearBuffers()
{
    _CurrentObjectBuffer->Clear();
    _CurrentValueBuffer->Clear();
    memset(_CurrentIdentifier, 0, 6);
    memset(_CurrentOperator, 0, 3);
}
