#include <__sympl_old/Sympl.hpp>
#include <fmt/format.h>
using namespace std;
SymplNamespace

void rec(SharedPtr<StringBuffer> str, int i)
{
    if (i == 10) {
        return;
    }

    str = StringBuffer::Alloc<StringBuffer>();
    str->Set(fmt::format("value = {0}", i).c_str());

    i++;
    rec(str, i);
}

int main()
{
    while (true) {
        {
            auto str = StringBuffer::Alloc<StringBuffer>();
            str->Set("whatup");
            rec(str, 0);
        }

		cout << "Mem Usage: " << MemPool::Instance()->GetMemUsage() << endl;
		cout << "Num Used Blocks: " << MemPool::Instance()->GetUsedBlocks() << endl;
		cout << "Num Unused Blocks: " << MemPool::Instance()->GetUnusedBlocks() << endl;

        std::string code;
        std::cout << "sympl> ";
        std::getline(cin, code);

		auto CodeBuffer = StringBuffer::Alloc<StringBuffer>();
		CodeBuffer->Set(code.c_str());
		CodeBuffer->Trim();

		if (CodeBuffer->Equals("exit")) {
			break;
		}
		if (CodeBuffer->Equals("blocks")) {
			std::vector<std::string> BlockNames;
			MemPool::Instance()->GetUsedBlockObjectNames(BlockNames);

			for (const auto& Name : BlockNames) {
				cout << Name << endl;
			}

			continue;
		}
		if (CodeBuffer->Equals("clrmem")) {
			MemPool::Instance()->FreeAllBlocks();
			continue;
		}
		if (CodeBuffer->Empty()) {
			continue;
		}

		auto vm = SymplVM::Alloc<SymplVM>();
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


