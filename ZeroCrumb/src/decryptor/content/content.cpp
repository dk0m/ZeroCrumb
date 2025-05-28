#include "content.h"

ContentReader::~ContentReader() {

	if (this->database) {
		sqlite3_close(this->database);
	}

	if (this->statement) {
		sqlite3_finalize(this->statement);
	}
}

BOOL ContentReader::isLocked() {
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

BOOL ContentReader::initSqliteDb() {
	if (this->isLocked()) {
		return FALSE; // deadlock file lock bypass will be integrated later
	}
	else {
		return sqlite3_open(this->filePath, &this->database) == SQLITE_OK;
	}
}

BOOL ContentReader::prepare(LPCSTR statement) {
	return sqlite3_prepare_v2(this->database, statement, -1, &this->statement, NULL) == SQLITE_OK;
}

BOOL ContentReader::step() {
	return sqlite3_step(this->statement) == SQLITE_ROW;
}

LPCSTR ContentReader::getText(int colIndex) {
	return (LPCSTR)sqlite3_column_text(this->statement, colIndex);
}

int ContentReader::getInt(int colIndex) {
	return sqlite3_column_int(this->statement, colIndex);
}

int ContentReader::getBytes(int colIndex) {
	return sqlite3_column_bytes(this->statement, colIndex);
}

