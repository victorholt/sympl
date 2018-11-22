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
#include <sympl/script/script_reader.h>
#include <sympl/core/string_buffer.h>
#include <sympl/core/registry.h>
#include <fmt/format.h>
sympl_namespaces

ScriptReader::ScriptReader()
{
    __Construct();
}

void ScriptReader::__Construct()
{
    _Buffer = mem_alloc_ref(StringBuffer);
    _ScriptToken = SymplRegistry.Get<ScriptToken>();
    _FilePath = "";
    _ScriptString = "";
}

bool ScriptReader::ReadFile(const char* filePath)
{
    _FilePath = filePath;

    std::ifstream inputStream (filePath, std::ifstream::in);
    if (!inputStream.is_open()) {
        return false;
    }

    inputStream.seekg(0, inputStream.end);
    auto fileLength = static_cast<size_t>(inputStream.tellg());
    inputStream.seekg(0, inputStream.beg);

    ProcessScript(inputStream, fileLength);

    // Close the stream.
    inputStream.close();
    return true;
}

bool ScriptReader::ReadString(const char* scriptString)
{
    _ScriptString = scriptString;
    std::istringstream inputStream (_ScriptString.c_str());

    inputStream.seekg(0, inputStream.end);
    auto fileLength = static_cast<size_t>(inputStream.tellg());
    inputStream.seekg(0, inputStream.beg);

    ProcessScript(inputStream, fileLength);
    return true;
}

void ScriptReader::SaveOutputTo(const char* filePath)
{
    std::ofstream outputStream(filePath, std::ofstream::out);
    outputStream << _Buffer->CStr();
    outputStream.close();
}

StringBuffer* ScriptReader::GetBuffer() const
{
    return _Buffer;
}

void ScriptReader::ProcessScript(std::istream& fileStream, size_t bufferLength)
{
    int currentLine = 1;
    char previousChar = '\0';
    bool recording = false;
    bool isComment = false;
    bool isBlockComment = false;
    std::locale loc;

    // Open the file.
    char currentChar;

    // Delete any previous script buffer.
    if (!IsNullObject(_Buffer)) {
        _Buffer->Clear();
    } else {
        _Buffer = mem_alloc_ref(StringBuffer);
    }
    _Buffer->Resize(2000);

    if (_AddLineNumbers) {
        _Buffer->Append(fmt::format("[|{0}|]", currentLine).c_str());
    }

    while (fileStream.get(currentChar)) {
        char nextChar = static_cast<char>(fileStream.peek());

        // Check if we're inside of a block comment.
        if ((!recording && currentChar == '/' && nextChar == '*') || isBlockComment) {
            isBlockComment = true;

            // Update the next line.
            if (currentChar == '\n') {
                currentLine++;

                // Add our line number.
                if (_AddLineNumbers) {
                    _Buffer->Append(fmt::format("[|{0}|]", currentLine).c_str());
                }
            }

            // Check if we need to close the comment.
            if (currentChar == '*' && nextChar == '/') {
                isBlockComment = false;

                // Move to the next character.
                fileStream.get();
            }

            continue;
        }

        // Check if we're inside of a comment.
        if ( (currentChar == '/' && nextChar == '/' && !recording) ||
             (currentChar == '#' && !recording) ||
             isComment) {
            isComment = true;
            if (currentChar == '\n') {
                currentLine++;
                isComment = false;

                // Add our line number.
                if (_AddLineNumbers) {
                    _Buffer->Append(fmt::format("[|{0}|]", currentLine).c_str());
                }
            }
            continue;
        }

        // Check if we need to start recording.
        if (currentChar == '"') {
            std::string encodedString;
            if (!recording && _ScriptToken->EncodeSpecialChar(currentChar, encodedString)) {
                _Buffer->Append(encodedString.c_str());
            }

            if (recording) {
                // Check for empty string.
                if (previousChar == '"' && currentChar == '"') {
                    _Buffer->Append("\\0");
                }

                if (_ScriptToken->EncodeSpecialChar(currentChar, encodedString)) {
                    _Buffer->Append(encodedString.c_str());
                }
            }
            previousChar = currentChar;
            recording = !recording;
            continue;
        }

        // Check if we have a whitespace character.
        if (!IsSpaceChar(currentChar, loc) || recording) {
            // First check if we're recording a special character.
            std::string encodedString;
            if (recording && _ScriptToken->EncodeSpecialChar(currentChar, encodedString)) {
                _Buffer->Append(encodedString);
            } else {
                // Append the # before an operator.
                if (!recording && !_ScriptToken->IsOperator(previousChar) &&
                    (_ScriptToken->IsOperator(previousChar) ||
                     _ScriptToken->IsIdentifier(previousChar)
                    )) {
                    if (_Buffer->Get(_Buffer->Length() - 1) != '#') {
                        _Buffer->AppendByte('#');
                    }
                }
                _Buffer->AppendByte(currentChar);

                // Append the # after an operator.
                if (!recording && !_ScriptToken->IsOperator(nextChar) &&
                    (_ScriptToken->IsOperator(nextChar) ||
                     _ScriptToken->IsIdentifier(nextChar)
                    )) {
                    _Buffer->AppendByte('#');
                }
            }
        }

        // Increase the line number we're on.
        if (IsSpaceChar(currentChar, loc)) {
            if (previousChar == '\n') {
                currentLine++;
                isComment = false;

                // Add our line number.
                if (_AddLineNumbers) {
                    _Buffer->Append(fmt::format("[|{0}|]", currentLine).c_str());
                }
            } else if (!recording && previousChar != ' ') {
                _Buffer->AppendByte('#');
            }
        }

        previousChar = currentChar;
    }
}

bool ScriptReader::Release()
{
    mem_free_ref(StringBuffer, _Buffer);
    mem_free_ref(ScriptToken, _ScriptToken);

    return true;
}
