#include "utils.h"

string utils::getEnvVar(LPCSTR name) {

	CHAR envVar[MAX_PATH];
	GetEnvironmentVariableA(name, envVar, MAX_PATH);

	return string(envVar);
}