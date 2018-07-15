#include <sympl/core/sympl.h>
#include <fmt/format.h>
using namespace std;
sympl_namespaces

int main()
{
//    auto sb1 = alloc_ref(StringBuffer);
//    sb1->Append("testing.bar");
//    cout << sb1->CStr() << endl;
//
//    if (sb1->Contains("testing")) {
//        cout << "String contains tested string" << endl;
//    } else {
//        cout << "String DOES NOT contain tested string" << endl;
//    }
//
//    auto firstOcc1 = sb1->SubstrFirstOccurence('.');
//    auto firstOcc2 = sb1->SubstrFirstOccurence("thi");
//
//    cout << firstOcc1 << endl;
//    cout << firstOcc2 << endl;
//
//    free_ref(sb1);
//    return 0;

//    mem_create_pool(StringBuffer, 200);
    mem_create_pool(StatementResolver, 100);
//    mem_create_pool(ScriptObject, 200);
//    mem_create_pool(ScriptMethod, 50);

    mem_create_object_pool(StatementEntry, 50);

    SymplRegistry.Register(mem_alloc_ref(ScriptToken));
    SymplRegistry.Register(mem_alloc_ref(StatementResolver));
    SymplRegistry.Register(mem_alloc_ref(EvalResolver));
    SymplRegistry.Register(mem_alloc_ref(MethodResolver));
    SymplRegistry.Register(mem_alloc_ref(ParenthResolver));

//    sympl_profile_start("memcheck");
//    for (int i = 0; i < 50000; i++) {
//        auto r = SymplRegistry.Get<EvalResolver>();
//    }
//    sympl_profile_stop_and_print("memcheck");
//    return 0;

    sympl_profile_start("program_load");
    auto program = ScriptVMInstance->LoadFile("../../examples/scripts/math.sym");
//    auto program = ScriptVMInstance->LoadString("");
    sympl_profile_stop_and_print("program_load");

//    sympl_profile_start("program_run");
    program->Run();
//    sympl_profile_stop_and_print("program_run");

    cout << "Memory Usage: " << MemPoolInstance.GetMemoryUsage() << endl;
    return 0;
}


