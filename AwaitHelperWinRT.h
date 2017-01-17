#pragma once
#include <experimental\resumable>
#include <wrl\client.h>
#include <ctxtcall.h>

// Portions here sourced from https://github.com/Microsoft/cppwinrt/blob/9e01842d6286da4ff3c279c8887df8f30c34d5fd/10.0.14393.0/winrt/base.h
// License is MIT license:
// C++ for the Windows Runtime(cppwinrt)
// 
// Copyright(c) Microsoft Corporation
// All rights reserved.
// 
// MIT License
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

namespace await_winrthelper
{
    __forceinline void check_hresult(const HRESULT result)
    {
        if (result != S_OK)
        {
            throw ::Platform::Exception::CreateException(result);
        }
    }

    struct thread_context
    {
        thread_context()
        {
            check_hresult(CoGetObjectContext(__uuidof(m_context), &m_context));
        }

        bool await_ready() const noexcept
        {
            return false;
        }

        void await_resume() const noexcept
        {
        }

        void await_suspend(std::experimental::coroutine_handle<> handle) const
        {
            ComCallData data = {};
            data.pUserDefined = handle.address();

            auto callback = [](ComCallData * data)
            {
                std::experimental::coroutine_handle<>::from_address(data->pUserDefined)();
                return S_OK;
            };

            check_hresult(m_context->ContextCallback(callback, &data, IID_ICallbackWithNoReentrancyToApplicationSTA, 5, nullptr));
        }

    private:
        Microsoft::WRL::ComPtr<IContextCallback> m_context;

    };
}