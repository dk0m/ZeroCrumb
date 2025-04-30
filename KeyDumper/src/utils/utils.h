#pragma once

#include<windows.h>
#include<winternl.h>
#include<ShlObj.h>
#include<string>

using std::wstring;

enum BrowserType {
	Chrome,
	Brave,
	Edge
};

struct BrowserLocalStateConfig {
	BrowserType type;
	wstring localState;
};


namespace utils {
	PPEB getPeb();

	wstring getMainModuleName();
	wstring getAppDataPath();

	BrowserLocalStateConfig getBrowserConfig();	
}