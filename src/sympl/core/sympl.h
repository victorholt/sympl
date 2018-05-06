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
#pragma once

#include <sympl/core/sympl_pch.h>

#include <sympl/core/alloc.h>
#include <sympl/core/sympl_ref.h>
#include <sympl/core/shared_ref.h>
#include <sympl/core/weak_ref.h>
#include <sympl/core/sympl_object.h>
#include <sympl/core/string_buffer.h>
#include <sympl/core/variant.h>
#include <sympl/core/thread.h>
#include <sympl/core/mutex.h>

#include <sympl/util/profiler.h>

#include <sympl/io/script_symbol.h>
#include <sympl/io/script_reader.h>
#include <sympl/io/script_parser.h>

#include <sympl/script/script_common.h>
#include <sympl/script/script_statement.h>
#include <sympl/script/sympl_vm.h>
#include <sympl/script/script_object.h>
