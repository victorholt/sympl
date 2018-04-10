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

extern size_t sympl_mem_size = 0;

void* sympl_new(size_t size)
{
    sympl_mem_size += size;
    return malloc(size);
}

void sympl_delete(void* src, size_t size)
{
    sympl_mem_size -= size;
    free(src);
}

sympl_ref *sympl_new_ref(void **dest, size_t size)
{
    sympl_mem_size += (size + sizeof(sympl_ref));

    sympl_ref *ref = malloc(sizeof(sympl_ref));

    ref->ref_count = 1;
    ref->data = malloc(size);
    ref->memsize = size + sizeof(sympl_ref);

    *dest = ref->data;

    return ref;
}

void sympl_delete_ref(sympl_ref **p_ref)
{
    sympl_ref* ref = *p_ref;
    ref->ref_count--;
    if (ref->ref_count >= 1) {
        return;
    }

    // Free up both the data and our reference.
    sympl_mem_size -= ref->memsize;
    free(ref->data);
    free(ref);

    ref = NULL;
}

void sympl_add_ref(sympl_ref **p_ref)
{
    sympl_ref *ref = *p_ref;
    if (ref != NULL) {
        ref->ref_count++;
    }
}

void sympl_remove_ref(sympl_ref *ref)
{
    sympl_delete_ref(&ref);
}
