// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#pragma warning(disable: 4265)
#pragma warning(disable: 4266)
#pragma warning(disable: 4062)

#include <windows.h>

#define XSAPI_API __stdcall


#if defined(__cplusplus)
extern "C" {
#endif


void
XSAPI_API
XuidStringToInt(
    PCWSTR pszXuid,
    _Out_ UINT64* pui64Xuid
    );

typedef VOID(XSAPI_API * XboxLiveAsyncCompletionRoutine) (
    _In_ HRESULT returnCode,
    _In_ void* context
    );

void
XSAPI_API
SignInAsync(
    _In_ PCWSTR xuid,
    _In_ XboxLiveAsyncCompletionRoutine completionRoutine,
    _In_opt_ void* context
    );

HRESULT
XSAPI_API
ProcessPendingAsync(
    _In_ BOOL waitForCompletion
    );


#if defined(__cplusplus)
} // end extern "C"
#endif // defined(__cplusplus)

  
class xbox_live_async_context
{
public:
    xbox_live_async_context() : m_returnCode(S_OK)
    {
    }
  
    HRESULT wait()
    {
        m_event.wait();
        return m_returnCode;
    }
  
    void set(HRESULT hr)
    {
        m_returnCode = hr;
        m_event.set();
    }
  
private:
    HRESULT m_returnCode;  
    pplx::event m_event;
};
  
  
void XSAPI_API XboxLiveAsyncCompletionRoutineImpl(
    _In_ HRESULT returnCode,
    _In_ void* context
    )
{
    xbox_live_async_context* asyncContext = (xbox_live_async_context*)context;
    if (asyncContext != nullptr)
    {
        asyncContext->set(returnCode);
    }
}
  

void
XSAPI_API
SignInAsyncWrap(
    _In_ PCWSTR xuid
    )
{
    xbox_live_async_context context;
    SignInAsync(
        xuid,
        XboxLiveAsyncCompletionRoutineImpl,
        static_cast<void*>(&context)
        );

    HRESULT hr = ProcessPendingAsync(true);
    if (SUCCEEDED(hr))
    {
        hr = context.wait();
    }
}

