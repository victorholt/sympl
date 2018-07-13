#include <sympl/core/sympl.h>
#include <fmt/format.h>
using namespace std;
sympl_namespaces

int main()
{
//    auto sb1 = alloc_ref(StringBuffer);
//    sb1->Append("testing this out");
//    cout << sb1->CStr() << endl;
//    free_ref(sb1);
//
//    return 0;

//    mem_create_pool(StringBuffer, 50);
//    mem_create_pool(StatementResolver, 50);
//    mem_create_pool(ScriptObject, 50);
//    mem_create_pool(ScriptMethod, 50);

//    mem_create_object_pool(StatementEntry, 50);

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


