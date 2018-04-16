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
#include "alloc.h"
#include "sympl_ref.h"
sympl_namespaces

Alloc* Alloc::_Instance = nullptr;

//template<class T>
//T* Alloc::Malloc(size_t size)
//{
//    T* ref = new T();
//    ref->_MemSize = size;
//    ref->_Data = malloc(size);
//
//    AddMemAllocated(sizeof(Ref) + ref->_MemSize);
//    return ref;
//}

//void Alloc::Free(SymplRef*& ref)
//{
//    if (!ref->Destroy()) {
//        return;
//    }
//
//    free(ref->_Data);
//    delete ref;
//    ref = nullptr;
//    RemoveMemAllocated(sizeof(Ref) + ref->_MemSize);
//}


