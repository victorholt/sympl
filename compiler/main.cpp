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

    MemPoolInstance.CreatePool<StringBuffer>(15000);
    MemPoolInstance.CreatePool<StatementResolver>(6000);
    MemPoolInstance.CreatePool<ScriptObject>();
    MemPoolInstance.CreatePool<ScriptMethod>();
    MemPoolInstance.CreatePool<CallbackMethod>();
    MemPoolInstance.CreatePool<IfMethod>();


    sympl_profile_start("math_program_load");
    auto program = ScriptVMInstance->LoadFile("../../examples/scripts/math.sym");
    sympl_profile_stop("math_program_load");
    sympl_profile_print("math_program_load");

    sympl_profile_start("math_program");
    program->Run();
    sympl_profile_stop("math_program");
    sympl_profile_print("math_program");

    cout << "Memory Usage: " << AllocInstance.GetMemoryUsage() << endl;
    return 0;
}


