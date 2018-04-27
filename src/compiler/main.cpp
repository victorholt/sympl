#include <sympl/core/sympl.h>
using namespace std;
sympl_namespaces

int main()
{
    sympl_profile_start("string_buffer");
    // Testing out the string buffer.
    Variant str = alloc_ref(StringBuffer);
    str.Set("Testing this string");
    str.GetStringBuffer()->AppendByte('!');
    cout << str.GetStringBuffer()->Str() << endl;
    cout << "Memory allocated: " << AllocInstance->GetMemAllocated() << endl;
    str.Free();
    // End string buffer test.
    sympl_profile_stop("string_buffer");
    sympl_profile_print("string_buffer");


    // Testing out threads.
    Mutex sharedIntMutex;
    Variant sharedInt = 0;
    auto thread1 = alloc_ref(Thread);
    thread1->SetCallback([&] {
        for (int i = 0; i < 5; i++) {
            sharedIntMutex.Lock();
            cout << "Thread1 - SharedInt value: " << sharedInt.GetInt() << endl;
            sharedInt.Set(sharedInt.GetInt() + 1);
            sharedIntMutex.Unlock();
        }
    });
    thread1->Start();

    auto thread2 = alloc_ref(Thread);
    thread2->SetCallback([&] {
        for (int i = 0; i < 5; i++) {
            sharedIntMutex.Lock();
            cout << "Thread2 - SharedInt value: " << sharedInt.GetInt() << endl;
            sharedInt.Set(sharedInt.GetInt() + 1);
            sharedIntMutex.Unlock();
        }
    });
    thread2->Start();


    // We don't want to join until they are finished because we would like
    // to test out the Mutex object.
    while (thread1->IsRunning() || thread2->IsRunning()) {}
    free_ref(Thread, thread1);
    free_ref(Thread, thread2);
    // End thread testing.

    // Testing the script reader.
    auto scriptReader = alloc_ref(ScriptReader);
    if (!scriptReader->ReadFile("../../examples/scripts/hello.sympl")) {
        printf("Failed to read example script!\n");
    } else {
        cout << scriptReader->GetBuffer()->CStr() << endl;
    }
    free_ref(ScriptReader, scriptReader);

    // Free our profiler.
    Sympl::Profiler* profiler = SymplProfiler;
    free_ref(Sympl::Profiler, profiler);
    cout << "Memory allocated: " << AllocInstance->GetMemAllocated() << endl;

    return 0;
}
