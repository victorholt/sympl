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
#include <sympl/core/mempool.h>
#include <sympl/core/object.h>

sympl_nsstart

struct ProfileRecord
{
    std::string Name;
    std::chrono::high_resolution_clock::time_point Start;
    std::chrono::high_resolution_clock::time_point End;
//    std::chrono::steady_clock::time_point Start;
//    std::chrono::steady_clock::time_point End;
    bool HasStarted;
};

class MemPool;
class MemPoolRef;
class MemPoolObject;
class MemPoolManager;

class SYMPL_API Profiler : public Object
{
    SYMPL_OBJECT(Profiler, Object);

private:
    /// Instance for the profiler.
    static Profiler* _Instance;

    /// Records saved for the profiler.
    std::unordered_map<std::string, ProfileRecord*> _Records;

    /// Whether or not profiling is enabled.
    bool _Enabled = true;

    //! Constructor.
    Profiler() { __Construct(); }

public:
    //! Destructor.
//    ~Profiler() {
//        if (!IsNullObject(_Instance)) {
////            _Instance->Release();
//
//            // Double-delete... our Alloc class should cache this
//            // and disregard attempting to remove it.
//            mem_free_ref(Profiler, _Instance);
//        }
//    }

    //! Called in place of the constructor.
    void __Construct() override;

    //! Returns the instance.
    //! \return Profiler
    static Profiler* GetInstance() {
        static Profiler instance;
        instance.SetIsStaticRef(true);

//        if (IsNullObject(_Instance)) {
//            _Instance = mem_alloc_ref(Profiler);
//            _Instance->SetIsStaticRef(true);
//        }

        return &instance;
    }

    //! Starts a record.
    //! \param recordName
    void Start(const std::string& recordName);

    //! Stops a record.
    //! \param recordName
    void Stop(const std::string& recordName);

    //! Resets the profiler record.
    void Reset(const std::string& recordName);

    //! Returns a record given the name.
    //! \param recordName
    //! \return ProfileRecord*
    ProfileRecord* GetRecord(const std::string& recordName);

    //! Prints the execution time.
    //! \param recordName
    void PrintExecutionTime(const std::string& recordName);

    //! Clears/removes all profile records.
    void Clear();

    //! Releases the object.
    bool Release() override;

    //! Sets whether or not to enable the profiler.
    //! \param enabled
    inline void SetEnabled(bool enabled) {
        _Enabled = enabled;
    }

    friend MemPool;
    friend MemPoolRef;
    friend MemPoolObject;
    friend MemPoolManager;
};

#define SymplProfiler Sympl::Profiler::GetInstance()
#define sympl_profile_start(recordName) SymplProfiler->Start(recordName)
#define sympl_profile_stop(recordName) SymplProfiler->Stop(recordName)
#define sympl_profile_print(recordName) SymplProfiler->PrintExecutionTime(recordName)
#define sympl_profile_stop_and_print(recordName) SymplProfiler->Stop(recordName); SymplProfiler->PrintExecutionTime(recordName)

sympl_nsend

