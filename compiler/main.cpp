#include <sympl/core/sympl.h>
#include <fmt/format.h>
using namespace std;
sympl_namespaces

int main()
{
    mem_create_object_pool(StatementEntry, 25);

    mem_create_pool(StringBuffer, 25);
    mem_create_pool(StatementResolver, 25);
    mem_create_pool(ScriptObject, 50);
    mem_create_pool(ScriptMethod, 50);

    mem_create_pool(EvalResolver, 25);

    SymplRegistry.Register(mem_alloc_ref(ScriptToken));
    SymplRegistry.Register(mem_alloc_ref(StatementResolver));
    SymplRegistry.Register(mem_alloc_ref(EvalResolver));
    SymplRegistry.Register(mem_alloc_ref(MethodResolver));
    SymplRegistry.Register(mem_alloc_ref(ParenthResolver));
    SymplRegistry.Register(mem_alloc_ref(ArrayResolver));

//    auto occurSb = mem_alloc_ref(StringBuffer);
//    occurSb->Append("arr = [\"one\", \"two\"];");
//    auto osb1Index = occurSb->FirstOccurrence('[');
//    auto osb2Index = occurSb->FirstOccurrence("\"two\"", 11, 25);
//    char c = occurSb->Get(osb2Index);
//    mem_free_ref(StringBuffer, occurSb);
//    return 0;

//    auto sb = mem_alloc_ref(StringBuffer);
//    sb->Append("Testing the replacement string");
//    sb->ReplaceStringAt(" on", 4);
//    cout << sb->CStr() << endl;
//
//    sb->Replace("replace", "deployments ");
//    cout << sb->CStr() << endl;
//
//    mem_free_ref(StringBuffer, sb);
//
//    std::string sstr = "Hello%em% How are you";
//    SymplRegistry.Get<ScriptToken>()->DecodeSpecialCharString(sstr.c_str(), sstr);
//    cout << sstr << endl;
//    return 0;

//    auto sb1 = mem_alloc_ref(StringBuffer);
//    auto sb2 = mem_alloc_ref(StringBuffer);
//
//    sb1->Append("text to replace");
//    sb2->Append("text to replace");
//
//    sympl_profile_start("string_replace_1");
//    for (int i = 0; i < 10000; i++) {
//        sb1->Replace("to re", "ib nl");
//        sb1->Replace("ib nl", "to re");
//    }
//    sympl_profile_stop_and_print("string_replace_1");
//
//    sympl_profile_start("string_replace_2");
//    for (int i = 0; i < 10000; i++) {
//        sb2->Replace2("to re", "ib nl");
//        sb2->Replace2("ib nl", "to re");
//    }
//    sympl_profile_stop_and_print("string_replace_2");
//
//    mem_free_ref(StringBuffer, sb1);
//    mem_free_ref(StringBuffer, sb2);
//
//    return 0;

//    sympl_profile_start("memcheck");
//    for (int i = 0; i < 50000; i++) {
//        auto r = SymplRegistry.Get<EvalResolver>();
//    }
//    sympl_profile_stop_and_print("memcheck");
//    return 0;

    sympl_profile_start("program_load");
    auto program = ScriptVMInstance->LoadFile("../../examples/scripts/arrays.sym");
//    auto program = ScriptVMInstance->LoadFile("../../examples/scripts/loops.sym");
//    auto program = ScriptVMInstance->LoadFile("../../examples/scripts/class.sym");
//    auto program = ScriptVMInstance->LoadFile("../../examples/scripts/math.sym");
//    auto program = ScriptVMInstance->LoadFile("../../examples/scripts/fib.sym");
//    auto program = ScriptVMInstance->LoadFile("../../examples/scripts/hello.sym");
    program = ScriptVMInstance->LoadString("var n = arr1[1]; printl(n);");
    sympl_profile_stop_and_print("program_load");

//    sympl_profile_start("program_run");
    program->Run();
//    sympl_profile_stop_and_print("program_run");

    cout << "Memory Usage: " << MemPoolInstance.GetMemoryUsage() << endl;
    return 0;
}


