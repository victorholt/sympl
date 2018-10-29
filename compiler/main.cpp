#include <sympl/core/sympl.h>
#include <fmt/format.h>
using namespace std;
sympl_namespaces

int main()
{
    // Any memory manager setting changes.
    //    MemPoolInstance.SetDebug(true);

    // Initialize the object addresses.
    MemPoolInstance.Initialize();

    // Create memory pools.
    mem_create_pool(StringBuffer, 500);

    mem_create_pool(StatementResolver, 500);
    mem_create_pool(StatementEntry, 500);

    mem_create_pool(ScriptObject, 500);
    mem_create_pool(ScriptMethod, 500);

    mem_create_pool(EvalResolver, 500);

    auto program = ScriptVMInstance.LoadFile("../../examples/scripts/math.sym");
    program->Run();

    return 0;
}


