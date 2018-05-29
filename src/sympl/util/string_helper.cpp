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
#include <sympl/util/string_helper.h>
sympl_namespaces

std::string StringHelper::GenerateGuid()
{
    auto guid = xg::newGuid();
    std::stringstream guidStream;
    guidStream << guid;
    return guidStream.str();
}

std::string StringHelper::GenerateRandomStr(size_t length, bool numbersOnly)
{
    auto randchar = []() -> char
    {
        const char an_charset[] =
                "0123456789"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "abcdefghijklmnopqrstuvwxyz";
        const short an_max_index = (sizeof(an_charset) - 1);
        return an_charset[ rand() % an_max_index ];
    };

    auto n_randchar = []() -> char
    {
        const char n_charset[] =
                "0123456789";
        const short n_max_index = (sizeof(n_charset) - 1);

        return n_charset[ rand() % n_max_index ];
    };

    std::string str(length, 0);

    if (numbersOnly) {
        std::generate_n( str.begin(), length, n_randchar );
    } else {
        std::generate_n( str.begin(), length, randchar );
    }

    return str;
}

std::string StringHelper::Replace(std::string find, std::string replace, std::string subject)
{
    std::string newStr;
    newStr.reserve(subject.size());
    newStr = subject;

    for (std::string::size_type i = 0; (i = newStr.find(find, i)) != std::string::npos;)
    {
        newStr.replace(i, find.length(), replace);
        i += replace.length();
    }

    return newStr;
}
