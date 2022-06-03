#include <sympl/Sympl.hpp>
#include <fmt/format.h>
using namespace std;
SymplNamespace

int main()
{
    auto vm = SymplVM::Alloc<SymplVM>();

    while (true) {
        std::string code;
        std::cout << "sympl> ";
        std::getline(cin, code);

        auto CodeBuffer = StringBuffer::Alloc<StringBuffer>();
        CodeBuffer->Set(code.c_str());
        CodeBuffer->Trim();

        if (CodeBuffer->Equals("exit"))
        {
            break;
        }
        if (CodeBuffer->Empty())
        {
            continue;
        }

        auto ret = vm->RunScript("<stdin>", CodeBuffer->CStr());
        auto value = std::get<0>(ret);
        auto error = std::get<1>(ret);

        if (error.IsValid())
        {
            cout << error->ToString() << endl;
        }
        else
        {
            auto ListResult = ObjectRef::CastTo<ListHandle>(value.Ptr());
            if (!ListResult->Elements.empty()) {
                if (ListResult->Elements.size() == 1) {
                    cout << ListResult->Elements[0]->ToString() << endl;
                }
                else {
                    cout << value->ToString() << endl;
                }
            }
        }
    }

    return 0;
}


