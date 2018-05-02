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

    // Shared pointers.
//    {
//        SharedRef<StringBuffer> sstr1 = alloc_ref(StringBuffer);
//        sstr1->Append("SharedRef String");
//        cout << "Shared str ref count: " << sstr1->RefCount() << endl;
//
//        SharedRef<StringBuffer> sstr2 = sstr1;
//        cout << "Shared str ref count: " << sstr1->RefCount() << endl;
//
//        if (sstr1.IsValid()) {
//            cout << "Shared str value: " << sstr1->Str() << endl;
//        }
//        cout << "Shared str ref count: " << sstr1->RefCount() << endl;
//    }

    // Testing the script reader.
    sympl_profile_start("script_reader");
    auto scriptReader = alloc_ref(ScriptReader);
    if (!scriptReader->ReadFile("../../examples/scripts/hello.sympl")) {
        printf("Failed to read example script!\n");
    } else {
        cout << scriptReader->GetBuffer()->CStr() << endl;
    }
    free_ref(ScriptReader, scriptReader);
    sympl_profile_stop("script_reader");
    sympl_profile_print("script_reader");

    // Test out the VM.
    Sympl::SymplVM* vm = SymplVMInstance;
    {
        auto obj = vm->CreateObject("MyObject");
        auto obj2 = vm->CreateObject("MyObject2", &obj);
        cout << obj.Print() << endl;
    }
    free_ref(Sympl::SymplVM, vm);

    // Free our profiler.
    Sympl::Profiler* profiler = SymplProfiler;
    free_ref(Sympl::Profiler, profiler);

    cout << "Memory allocated: " << AllocInstance->GetMemAllocated() << endl;
    return 0;
}
