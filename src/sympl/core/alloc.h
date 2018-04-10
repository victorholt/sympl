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
#ifndef __SYMPL_ALLOC_H__
#define __SYMPL_ALLOC_H__

#include "sympl_pch.h"

static size_t sympl_mem_size;

typedef struct sympl_ref {
    void        *data;
    unsigned    ref_count;
    ulong64     memsize;
} sympl_ref;

// Allocates memory.
void* sympl_new(size_t size);

// Frees memory.
void sympl_delete(void* src, size_t size);

// Allocates memory for a reference.
sympl_ref *sympl_new_ref(void **dest, size_t size);

// Frees memory for a reference.
void sympl_delete_ref(sympl_ref **p_ref);

// Increments the reference count.
void sympl_add_ref(sympl_ref **p_ref);

// Decrements the reference count.
void sympl_remove_ref(sympl_ref *ref);

#endif