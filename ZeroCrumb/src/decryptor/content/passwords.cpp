#include "content.h"

void PasswordReader::populatePasswords() {

	while (this->step()) {
		auto name = this->getText(0);
		auto site = this->getText(1);

		auto blobSize = this->getBytes(2);
		auto encPassword = this->getBlob<PBYTE>(2);

		vector<BYTE> passwordsVec(encPassword, encPassword + blobSize);

		string password = decryptor::decrypt(key, passwordsVec);

		auto entry = new PasswordEntry{ name, site, password };

		this->passwords.push_back(entry);
	}
}