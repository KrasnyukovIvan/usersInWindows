#include <napi.h>
#ifndef UNICODE
#define UNICODE
#endif
#pragma comment(lib, "netapi32.lib")

#include <stdio.h>
#include <assert.h>
#include <windows.h>
#include <lm.h>
#include <clocale>
#include<string.h>

#include <codecvt>


Napi::Boolean Method(const Napi::CallbackInfo& info) {
        setlocale(LC_ALL, "");

    Napi::Env env = info.Env();

    std::string searchName = info[0].ToString().Utf8Value();

    LPUSER_INFO_0 pBuf = NULL;
    LPUSER_INFO_0 pTmpBuf;
    DWORD dwLevel = 0;
    DWORD dwPrefMaxLen = MAX_PREFERRED_LENGTH;
    DWORD dwEntriesRead = 0;
    DWORD dwTotalEntries = 0;
    DWORD dwResumeHandle = 0;
    DWORD i;
    DWORD dwTotalCount = 0;
    NET_API_STATUS nStatus;
    LPTSTR pszServerName = NULL;

    do{
        nStatus = NetUserEnum((LPCWSTR)pszServerName,
        dwLevel,
        FILTER_NORMAL_ACCOUNT,
        (LPBYTE*)&pBuf,
        dwPrefMaxLen,
        &dwEntriesRead,
        &dwTotalEntries,
        &dwResumeHandle);
        if ((nStatus == NERR_Success) || (nStatus == ERROR_MORE_DATA))
        {
            if ((pTmpBuf = pBuf) != NULL)
            {
                for (i = 0; (i < dwEntriesRead); i++)
                {
                    assert(pTmpBuf != NULL);

                    if (pTmpBuf == NULL)
                    {
                        fprintf(stderr, "An access violation has occurred\n");
                        break;
                    }
                    std::wstring wsUserName(pTmpBuf->usri0_name);

                    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
                    std::string UserName = converter.to_bytes( wsUserName );

                    if(strcmp(searchName.c_str(), UserName.c_str()) == 0){
                        return Napi::Boolean::New(env, true);
                    }

                    pTmpBuf++;
                    dwTotalCount++;
                }
            }
        } else
            fprintf(stderr, "A system error has occurred: %d\n", nStatus);

        if (pBuf != NULL)
        {
            NetApiBufferFree(pBuf);
            pBuf = NULL;
        }
    }while (nStatus == ERROR_MORE_DATA);
    return Napi::Boolean::New(env, false);
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set(Napi::String::New(env, "userExists"),
              Napi::Function::New(env, Method));
  return exports;
}

NODE_API_MODULE(addon, Init)

