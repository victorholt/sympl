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

#include <fmt/format.h>

#include <urho3d/container/Swap.h>
#include <urho3d/container/VectorBase.h>
#include <urho3d/container/Vector.h>

#include <sympl/core/sympl_pch.h>

#include <sympl/core/mempool.h>
#include <sympl/core/registry.h>
#include <sympl/core/ref.h>
#include <sympl/core/object.h>
#include <sympl/core/shared_ptr.h>
#include <sympl/core/weak_ptr.h>
#include <sympl/core/string_buffer.h>
#include <sympl/core/variant.h>
#include <sympl/core/thread.h>
#include <sympl/core/mutex.h>

#include <sympl/util/profiler.h>

#include <sympl/script/script_token.h>
#include <sympl/script/script_reader.h>
//#include <sympl/script/script_parser.h>

#include <sympl/script/script_common.h>
//#include <sympl/script/script_context.h>
//#include <sympl/script/statement_resolver.h>
#include <sympl/script/script_vm.h>
//#include <sympl/script/interpreter.h>
#include <sympl/script/script_object_ref.h>
//#include <sympl/script/script_object.h>
//#include <sympl/script/script_method.h>
//#include <sympl/script/methods/callback_method.h>
//#include <sympl/script/methods/if_method.h>
//#include <sympl/script/methods/while_method.h>
