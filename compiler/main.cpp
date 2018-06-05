#include <sympl/core/sympl.h>
#include <fmt/format.h>
using namespace std;
sympl_namespaces

int main()
{
//    sympl_profile_start("math_program_load");
    auto program = ScriptVMInstance->LoadFile("../../examples/scripts/math.sym");
//    sympl_profile_stop("math_program_load");
//    sympl_profile_print("math_program_load");

//    sympl_profile_start("math_program");
    program->Run();
//    sympl_profile_stop("math_program");
//    sympl_profile_print("math_program");

    cout << "Memory Usage: " << AllocInstance.GetMemoryUsage() << endl;
    return 0;
}


