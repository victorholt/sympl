#include <sympl/core/sympl.h>
#include <fmt/format.h>
using namespace std;
sympl_namespaces

int main()
{
    // Test generating a guid.
    auto guid = xg::newGuid();
    std::stringstream guidStream;
    guidStream << guid;

    sympl_profile_start("string_buffer");
    // Testing out the string buffer.
    Variant str = alloc_ref(StringBuffer);
    str.Set("Generated Guid: ");
    str.GetStringBuffer()->Append(guidStream.str().c_str());
    str.GetStringBuffer()->AppendByte('.');

    auto buffer = str.GetStringBuffer();
    buffer->Prepend("Hello there! ");
    buffer->PrependByte(' ');
    buffer->PrependByte('$');

    cout << buffer->Str() << endl;
    str.Clear();
    // End string buffer test.
    sympl_profile_stop("string_buffer");
    sympl_profile_print("string_buffer");

    cout << "Memory allocated: " << AllocInstance->GetMemAllocated() << endl;

    // Testing out threads.
    // Mutex sharedIntMutex;
    // Variant sharedInt = 0;
    // auto thread1 = alloc_ref(Thread);
    // thread1->SetCallback([&] {
    //     for (int i = 0; i < 5; i++) {
    //         sharedIntMutex.Lock();
    //         cout << "Thread1 - SharedInt value: " << sharedInt.GetInt() << endl;
    //         sharedInt.Set(sharedInt.GetInt() + 1);
    //         sharedIntMutex.Unlock();
    //     }
    // });
    // thread1->Start();

    // auto thread2 = alloc_ref(Thread);
    // thread2->SetCallback([&] {
    //     for (int i = 0; i < 5; i++) {
    //         sharedIntMutex.Lock();
    //         cout << "Thread2 - SharedInt value: " << sharedInt.GetInt() << endl;
    //         sharedInt.Set(sharedInt.GetInt() + 1);
    //         sharedIntMutex.Unlock();
    //     }
    // });
    // thread2->Start();

    // We don't want to join until they are finished because we would like
    // to test out the Mutex object.
    // while (thread1->IsRunning() || thread2->IsRunning()) {}
    // free_ref(Thread, thread1);
    // free_ref(Thread, thread2);
    // End thread testing.

    SharedRef<Interpreter> program = SymplVMInstance->LoadFile("../../examples/scripts/fib.sym");
    sympl_profile_start("script_interpreter");
    program->Run();
    sympl_profile_stop("script_interpreter");
    sympl_profile_print("script_interpreter");
    program.Release();

    // cout << SymplVMInstance->PrintObjects() << endl;

    // auto xVar = SymplVMInstance->FindObjectByPath("x");
    // auto yVar = SymplVMInstance->FindObjectByPath("y");
    // auto zVar = SymplVMInstance->FindObjectByPath("z");
    // auto argVar = SymplVMInstance->FindObjectByPath("foo.args.n");

    // if (!xVar->IsEmpty()) {
    //     Variant value = xVar->GetValue();
    //     cout << "x value is " << value.AsString() << fmt::format(" ({0}) ", value.GetTypeAsString()) << endl;
    // }

    // if (!IsNullObject(yVar)) {
    //     Variant value = yVar->GetValue();
    //     cout << "y value is " << value.AsString() << fmt::format(" ({0}) ", value.GetTypeAsString()) << endl;
    // }

    // if (!IsNullObject(zVar)) {
    //     Variant value = zVar->GetValue();
    //     cout << "z value is " << value.AsString() << fmt::format(" ({0}) ", value.GetTypeAsString()) << endl;
    // }

    // if (!IsNullObject(argVar)) {
    //     cout << "foo.args.n is " << argVar->Print() << endl;
    // }

    // Free our VM.
    Sympl::SymplVM* vm = SymplVMInstance;
    free_ref(Sympl::SymplVM, vm);

    // Free our profiler.
    Sympl::Profiler* profiler = SymplProfiler;
    free_ref(Sympl::Profiler, profiler);

    cout << "Memory allocated: " << AllocInstance->GetMemAllocated() << endl;
    cout << AllocInstance->PrintRefs() << endl;
    return 0;
}


