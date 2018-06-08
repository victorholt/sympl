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

    mem_create_pool(StringBuffer, 200);
    mem_create_pool(StatementResolver, 200);
    mem_create_pool(ScriptObject, 200);
    mem_create_pool(ScriptMethod, 200);
//    mem_create_pool(CallbackMethod, 200);
//    mem_create_pool(IfMethod, 200);
//    mem_create_pool(WhileMethod, 200);

    mem_create_object_pool(EvalResolver, 50);
    mem_create_object_pool(MethodResolver, 50);
    mem_create_object_pool(ParenthResolver, 50);

    sympl_profile_start("program_load");
    auto program = ScriptVMInstance->LoadFile("../../examples/scripts/math.sym");
    sympl_profile_stop("program_load");
    sympl_profile_print("program_load");

    program->Run();

    cout << "Memory Usage: " << AllocInstance.GetMemoryUsage() << endl;
    return 0;
}


