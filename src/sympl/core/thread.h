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

#include "sympl_pch.h"
#include "sympl_object.h"

sympl_nsstart

typedef std::function<void()> SymplThreadCallback;

class SYMPL_API Thread : public Object {
protected:
    /// Reference to the system thread.
    std::thread _SystemThread;

    /// Reference to the thread callback.
    SymplThreadCallback _Callback;

    /// Whether or not the thread has started.
    bool _ThreadStarted = false;

    /// Whether or not to pause the thread.
    bool _IsPaused = false;

    /// Whether or not the thread is a single-run thread.
    bool _IsSingleRun = true;

    /// Whether or not the thread has joined to the main thread.
    bool _IsJoined = false;

    /// Whether or not the thread is running.
    bool _IsRunning = false;

    /// Whether or not the thread has a callback.
    bool _HasCallback = false;

public:
    //! Constructor.
    Thread();

    //! Destructor.
    ~Thread();

    //! Starts the thread.
    void Start();

    //! Joins the thread to the main thread.
    void Join();

    //! Pauses a non single-run thread.
    void Pause();

    //! Stops a non single-run thread.
    void Stop();

    //! Sets the callback for the thread.
    //! \param callback
    void SetCallback(SymplThreadCallback callback);

    //! Returns whether or not the thread is running.
    //! \return
    inline bool IsRunning() const { return _IsRunning; }
};

sympl_nsend