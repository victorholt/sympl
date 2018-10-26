#include <sympl/core/sympl.h>
#include <fmt/format.h>
using namespace std;
sympl_namespaces

int main()
{
    mem_create_pool(StringBuffer, 100);
    mem_create_pool(StatementResolver, 100);

//    mem_create_object_pool(StatementEntry, 25);

//    mem_create_pool(MethodResolver, 25);
//    mem_create_pool(ArrayResolver, 25);
//    mem_create_pool(ScriptObject, 50);
//    mem_create_pool(ScriptMethod, 50);

//    mem_create_pool(EvalResolver, 25);

    ScriptVMInstance.GetGlobalObject(); // Creates our global object.

    auto program = ScriptVMInstance.LoadFile("../../examples/scripts/math.sym");
    program->Run();

    return 0;
}


