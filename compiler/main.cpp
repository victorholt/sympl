#include <sympl/core/sympl.h>
#include <fmt/format.h>
using namespace std;
sympl_namespaces

int main()
{
    mem_create_pool(StringBuffer, 25);
    auto global = ScriptVMInstance.GetGlobalObject();
    cout << "Global Object: " << global->GetObjectAddress() << endl;

    sympl_profile_start("program_run");
    auto program = ScriptVMInstance.LoadFile("../../examples/scripts/hello.sym");
    program->Run();
    sympl_profile_stop_and_print("program_run");

    // Find the variable and print it out.
    auto greetingVar = ScriptVMInstance.FindObjectByPath("greeting");
    cout << greetingVar->GetValue().AsString() << endl;

    cout << "Memory Usage: " << MemPoolInstance.GetMemoryUsage() << endl;

    return 0;
}


