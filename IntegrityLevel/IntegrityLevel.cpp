#include "stdafx.h"

// Sample code written by mikehow@microsoft.com
// 5/17/2018

// Spins up a process at a given integrity level
// which must be equal to or less than the parent process integrity level
// The valid integrity level SIDs are:
//   Low		S-1-16-4096
//   Medium		S-1-16-8192
//	 Medium+	S-1-16-8448
//	 High		S-1-16-12288
DWORD CreateProcessAtIntegrityLevel(
					_In_z_ const wchar_t *wszProcess, 
					_In_z_ const wchar_t *wszIntegritySid) {

	DWORD                 dwRet = 0;
	HANDLE                hToken = NULL;
	HANDLE                hNewToken = NULL;
	PSID                  pIntegritySid = NULL;
	TOKEN_MANDATORY_LABEL TIL = { 0 };
	PROCESS_INFORMATION   pi = { 0 };
	STARTUPINFO           si = { 0 };
	si.cb = sizeof STARTUPINFO;

	if (!wszProcess || !wszIntegritySid || !wcslen(wszProcess) || !wcslen(wszIntegritySid))
		return ERROR_BAD_ARGUMENTS;

	try {

		// Get current process token
		if (!OpenProcessToken(GetCurrentProcess(),
			TOKEN_DUPLICATE | TOKEN_ADJUST_DEFAULT | TOKEN_QUERY | TOKEN_ASSIGN_PRIMARY,
			&hToken))
			throw GetLastError();

		// We need to get a copy of the token
		if (!DuplicateTokenEx(hToken,
			0,
			NULL,
			SecurityImpersonation,
			TokenPrimary,
			&hNewToken))
			throw GetLastError();

		if (!ConvertStringSidToSid(wszIntegritySid, &pIntegritySid))
			throw GetLastError();

		TIL.Label.Attributes = SE_GROUP_INTEGRITY;
		TIL.Label.Sid = pIntegritySid;

		// Set the process integrity level
		if (!SetTokenInformation(hNewToken,
			TokenIntegrityLevel,
			&TIL,
			sizeof(TOKEN_MANDATORY_LABEL) + GetLengthSid(pIntegritySid)))
			throw GetLastError();

		wchar_t wszPath[MAX_PATH];
		wcscpy_s(wszPath, wszProcess);

		// Create the new process at the new integrity level
		if (!CreateProcessAsUser(hNewToken,
			NULL,
			(LPWSTR)wszPath,
			NULL,
			NULL,
			FALSE,
			CREATE_NEW_CONSOLE,
			NULL,
			NULL,
			&si,
			&pi))
				throw GetLastError();
	}
	catch (DWORD dwErr) {
		dwRet = dwErr;
	}

	if (pi.hProcess != NULL)
		CloseHandle(pi.hProcess);

	if (pi.hThread != NULL)
		CloseHandle(pi.hThread);

	if (pIntegritySid)
		LocalFree(pIntegritySid);

	if (hNewToken != NULL)
		CloseHandle(hNewToken);

	if (hToken != NULL)
		CloseHandle(hToken);

	return dwRet;
}

int main()
{
	const wchar_t *wszFilename = L"c:\\windows\\system32\\notepad.exe";
	const wchar_t *wszIntegrityLevel = L"S-1-16-4096";

	CreateProcessAtIntegrityLevel(wszFilename, wszIntegrityLevel);
	
	return 0;
}

