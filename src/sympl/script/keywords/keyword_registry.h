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

#include <sympl/core/sympl_pch.h>
#include <sympl/core/sympl_object.h>
#include <sympl/core/shared_ref.h>

#include <sympl/script/keywords/keyword_handle.h>

sympl_nsstart

class SYMPL_API KeywordRegistry : public Object
{
    SYMPL_OBJECT(KeywordRegistry, Object);

private:
    /// List of keywords available.
    std::unordered_map<std::string, SharedRef<KeywordHandle> >_Keywords;

    //! Initializes the system keywords.
    void _Initialize();

public:
    //! Constructor.
    KeywordRegistry();

    //! Destructor.
    ~KeywordRegistry() override;

    //! Adds a keyword to the registry.
    //! \param keyword
    //! \param handle
    void AddKeyword(const char* keyword, KeywordHandle* handle);

    //! Finds and returns a keyword.
    //! \param keyword
    //! \return KeywordHandle*
    KeywordHandle* FindKeyword(const char* keyword);

    //! Returns whether a keyword was found.
    //! \param keyword
    //! \param handle
    //! \return bool
    bool TryFindKeyword(const char* keyword, KeywordHandle*& handle);
};

sympl_nsend
