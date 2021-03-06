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
#include <sympl/util/profiler.h>

#include <fmt/format.h>
sympl_namespaces

Profiler* Profiler::_Instance = nullptr;

void Profiler::__Construct()
{
    _Enabled = true;
}

void Profiler::Start(const std::string& recordName)
{
    if (!_Enabled) return;
    auto record = GetRecord(recordName);
    record->Start = std::chrono::high_resolution_clock::now();
    record->HasStarted = true;
}

void Profiler::Stop(const std::string& recordName)
{
    if (!_Enabled) return;
    auto record = GetRecord(recordName);
    record->End = std::chrono::high_resolution_clock::now();
}

void Profiler::Reset(const std::string& recordName)
{
    auto record = GetRecord(recordName);
    record->Start = std::chrono::high_resolution_clock::now();
    record->End = std::chrono::high_resolution_clock::now();
}

ProfileRecord* Profiler::GetRecord(const std::string& recordName)
{
    if (!_Enabled) return nullptr;

    auto record = _Records.find(recordName);
    if (record == _Records.end()) {
        auto record = alloc_bytes(ProfileRecord);
        record->HasStarted = false;
        _Records[recordName] = record;
        return record;
    }
    return record->second;
}

void Profiler::PrintExecutionTime(const std::string& recordName)
{
    if (!_Enabled) return;
    auto record = GetRecord(recordName);

    if (!record->HasStarted) {
        printf("Profile for %s has not started\n!", recordName.c_str());
        return;
    }

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            record->End - record->Start
    ).count();

//    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
//    double seconds = duration / 1000000.0f;
    double seconds = duration / 1000.0f;
//    std::cout << recordName << " Execution Time: " << std::setprecision(15) << seconds << " seconds" << std::endl;
    std::cout << recordName << " Execution Time: " << fmt::format("{:.{}f}", seconds, 15) << " seconds" << std::endl;
//    printf("%s Execution Time: %d seconds\n", recordName.c_str(), seconds);

    Reset(recordName);
}

void Profiler::Clear()
{
    for (auto recordIt : _Records) {
        free_bytes(recordIt.second);
    }
    _Records.clear();
}

bool Profiler::Release()
{
    Clear();
    return true;
}
