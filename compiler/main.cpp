#include <sympl/core/sympl.h>
#include <fmt/format.h>
using namespace std;
sympl_namespaces

int main()
{
//    auto str = alloc_ref(StringBuffer);
//    str->Append("testing");
//    cout << str->CStr() << endl;
//    return 0;

    sympl_profile_start("math_program_load");
    auto program = ScriptVMInstance->LoadFile("../../examples/scripts/math.sym");
    sympl_profile_stop("math_program_load");
    sympl_profile_print("math_program_load");

    sympl_profile_start("math_program");
    program->Run();
    sympl_profile_stop("math_program");
    sympl_profile_print("math_program");

    auto var = ScriptVMInstance->FindObjectByPath("x");
    if (!var->IsEmpty()) {
        cout << "x = " << var->GetValue().AsString() << " (" << var->GetValue().GetTypeAsString() << ")" << endl;
    }

    cout << "Memory Usage: " << AllocInstance->GetMemoryUsage() << endl;
    return 0;
}


