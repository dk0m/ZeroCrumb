#pragma once
#include<string>

enum BrowserType {
	Chrome,
	Brave,
	Edge
};
struct BrowserPathConfig {
	BrowserType type;
	std::wstring exePath;
};

namespace config {
	
	static std::wstring keyDumperPath = L"./KeyDumper.exe";

	static BrowserPathConfig chrome = BrowserPathConfig{
		BrowserType::Chrome,
		L"C:\\Program Files\\Google\\Chrome\\Application\\chrome.exe"
	};

	static BrowserPathConfig brave = BrowserPathConfig{
		BrowserType::Brave,
		L"C:\\Program Files\\BraveSoftware\\Brave-Browser\\Application\\brave.exe"
	};

	static BrowserPathConfig edge = BrowserPathConfig{
		BrowserType::Edge,
		L"C:\\Program Files (x86)\\Microsoft\\Edge\\Application\\msedge.exe"
	};
}