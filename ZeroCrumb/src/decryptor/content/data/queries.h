#pragma once

namespace queries {
	static const char* cookies = "SELECT name, host_key, path, encrypted_value FROM cookies";
	static const char* passwords = "SELECT username_value, origin_url, password_value FROM logins";
}

