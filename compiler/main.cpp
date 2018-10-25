#include <sympl/core/sympl.h>
#include <fmt/format.h>
using namespace std;
sympl_namespaces

int main()
{
    mem_create_pool(StringBuffer, 25);
    ScriptVMInstance.GetGlobalObject(); // Creates our global object.

    auto program = ScriptVMInstance.LoadFile("../../examples/scripts/arrays.sym");
    program->Run();

    return 0;
}


