#include <sympl/core/sympl.h>
#include <fmt/format.h>
using namespace std;
sympl_namespaces

int main()
{
    mem_create_pool(StringBuffer, 25);
    ScriptVMInstance.GetGlobalObject(); // Creates our global object.

    auto program = ScriptVMInstance.LoadFile("../../examples/scripts/fib.sym");
    program->Run();

    cout << "Memory Usage: " << MemPoolInstance.GetMemoryUsage() << endl;

    return 0;
}


