/**********************************************************
 * Author:  GameSencha, LLC
 * The MIT License (MIT)
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a
 *  copy of this software and associated documentation files (the "Software"),
 *  to deal in the Software without restriction, including without limitation
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *  and/or sell copies of the Software, and to permit persons to whom the
 *  Software is furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 *  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 *  DEALINGS IN THE SOFTWARE.
 *
 **********************************************************/
#include <sympl/script/script_vm.h>
#include <sympl/script/script_object_ref.h>

#include <sympl/util/string_helper.h>
sympl_namespaces

ScriptVM::ScriptVM()
{
    __Construct();
}

ScriptVM::~ScriptVM()
{
}

void ScriptVM::__Construct()
{
    _BuildAddresses();

    // Create the global object.
    _GlobalObject = CreateObject();
    _GlobalObject->Initialize("root", "root", ScriptObjectType::Object);
}

ScriptObjectRef* ScriptVM::CreateObject()
{
    auto ref = mem_alloc_ref(ScriptObjectRef);
    ref->SetObjectAddress(ReserveObjectAddress());
    _ObjectMap[ref->GetObjectAddress()] = ref;

    return ref;
}

void ScriptVM::_BuildAddresses()
{
    _ObjectMap.clear();
    _ObjectMap.reserve(_MaxObjectAddresses);

    _AvailableObjectAddresses.clear();
    _AvailableObjectAddresses.reserve(_MaxObjectAddresses);

    for (size_t i = 0; i < _MaxObjectAddresses; i++) {
        auto address = StringHelper::GenerateRandomStr(64);
        _AvailableObjectAddresses.push_back(address);
    }
}

std::string ScriptVM::ReserveObjectAddress()
{
    auto result = _AvailableObjectAddresses.back();
    _AvailableObjectAddresses.pop_back();
    return result;
}

void ScriptVM::ReleaseObjectAddress(const std::string& address)
{
    sympl_assert(IsAvailableObjectAddress(address), "Attempted to release an object address that's already available!");
    _ObjectMap.erase(address);
    _AvailableObjectAddresses.push_back(address);
}

bool ScriptVM::IsAvailableObjectAddress(const std::string& address)
{
    auto addressIt = std::begin(_AvailableObjectAddresses);
    while (addressIt != std::end(_AvailableObjectAddresses)) {
        if (*addressIt == address) {
            return false;
        }
        ++addressIt;
    }
    return true;
}
