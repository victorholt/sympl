#include <sympl/core/sympl.h>
#include <fmt/format.h>
using namespace std;
sympl_namespaces

int main()
{
    // Any memory manager setting changes.
//    MemPoolInstance.SetDebug(true);
    MemPoolInstance.SetMaxObjectAddresses(10000);

    // Initialize the object addresses.
    MemPoolInstance.Initialize();

    // Create memory pools.
    mem_create_pool(StringBuffer, 200);

    mem_create_pool(StatementResolver, 200);
    mem_create_pool(StatementEntry, 200);

//    mem_create_pool(ScriptObject, 1000);
//    mem_create_pool(ScriptMethod, 1000);

//    mem_create_pool(EvalResolver, 100);

    auto program = ScriptVMInstance.LoadFile("../../examples/scripts/math.sym");
    program->Run();

    return 0;
}


