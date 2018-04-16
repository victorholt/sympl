#include <sympl/core/sympl.h>
using namespace std;
sympl_namespaces

int main()
{
    auto obj = alloc_ref(StringBuffer);
    obj->Append("Hello, I'm a StringBuffer!");

    cout << obj->Str() << endl;
    free_ref(obj);

//    Var<char*> str = "Testing this string";
    Var<char*> str;
    str.Set("Testing this string");
    cout << str.Get() << endl;

    return 0;
}
