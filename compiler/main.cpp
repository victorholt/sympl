#include <sympl/core/sympl.h>
#include <fmt/format.h>
using namespace std;
sympl_namespaces

int main()
{
    mem_create_pool(StringBuffer, 25);
    sympl_profile_start("program_run");

//    auto maxObjects = ScriptVMInstance.GetMaxObjectAddresses();
//    cout << "Global Object: " << ScriptVMInstance.GetGlobalObject()->GetObjectAddress() << endl;

    cout << "Memory Usage: " << MemPoolInstance.GetMemoryUsage() << endl;
    sympl_profile_stop_and_print("program_run");

    return 0;
}


