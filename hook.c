#include <Windows.h>
#include <Shlobj.h>
#include "proxy.h"
#include "minhook/include/MinHook.h"
#include <stdio.h>

BOOL passwordSaved = FALSE;
BOOL certSaved = FALSE;
char password[1024];
char certSuffix[] = "\\AltServer-Cert.p12";
char passSuffix[] = "\\AltServer-Cert-Pass.txt";

typedef char *(*sig_PKCS12_create)(char *pass, char *name, char *pkey, char *cert,
								   char *ca, int nid_key, int nid_cert, int iter,
								   int mac_iter, int keytype);
typedef size_t (*sig_fwrite)(const void *buffer, size_t size, size_t count, void *stream);

sig_PKCS12_create orig_PKCS12_create = NULL;
sig_fwrite orig_fwrite = NULL;

char *detour_PKCS12_create(char *pass, char *name, char *pkey, char *cert,
						   char *ca, int nid_key, int nid_cert, int iter,
						   int mac_iter, int keytype) {
	if (!passwordSaved && strlen(pass) > 0) {
		passwordSaved = TRUE;
		strcpy_s(password, sizeof(password), pass);
	}
	return orig_PKCS12_create(pass, name, pkey, cert, ca, nid_key, nid_cert, iter, mac_iter, keytype);
}
size_t detour_fwrite(const char *buffer, size_t size, size_t count, void *stream) {
	// match P12 magic header bytes
	if (passwordSaved && !certSaved && count > 2 && buffer[0] == (char)0x30 && buffer[1] == (char)0x82) {
		certSaved = TRUE;
		if (MH_DisableHook(MH_ALL_HOOKS) == MH_OK) {
			char certPath[1024];
			if (SHGetSpecialFolderPathA(HWND_DESKTOP, certPath, CSIDL_DESKTOP, FALSE)) {
				char passPath[1024];
				strcpy_s(passPath, sizeof(passPath), certPath);
				strcat_s(certPath, sizeof(certPath), certSuffix);
				strcat_s(passPath, sizeof(passPath), passSuffix);
				FILE *pFile;
				pFile = fopen(certPath, "wb");
				fwrite(buffer, size, count, pFile);
				fclose(pFile);
				pFile = fopen(passPath, "wb");
				fwrite(password, sizeof(char), strlen(password), pFile);
				fclose(pFile);
				MessageBox(NULL, "Certificate successfully saved to Desktop.", "Success", MB_OK);
			} else {
				MessageBox(NULL, "Error getting path to Desktop.", "Error", MB_OK);
			}
		} else {
			MessageBox(NULL, "Error disabling hooks.", "Error", MB_OK);
		}
	}
	return orig_fwrite(buffer, size, count, stream);
}

BOOL doHook() {
	if (MH_Initialize() != MH_OK) {
		return FALSE;
	}
	if (MH_CreateHookApi(L"libeay32.dll", "PKCS12_create",
						 (LPVOID)(detour_PKCS12_create),
						 (LPVOID *)(&orig_PKCS12_create)) != MH_OK) {
		return FALSE;
	}
	if (MH_CreateHookApi(L"ucrtbase.dll", "fwrite",
						 (LPVOID)(detour_fwrite),
						 (LPVOID *)(&orig_fwrite)) != MH_OK) {
		return FALSE;
	}
	if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK) {
		return FALSE;
	}
	return TRUE;
}

BOOL WINAPI DllMain(HMODULE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
	if (fdwReason == DLL_PROCESS_ATTACH) {
		SourceInit();
		if (doHook()) {
			MessageBox(NULL, "Hooks initialized.", "Success", MB_OK);
		} else {
			MessageBox(NULL, "Something went wrong.", "Error", MB_OK);
		}
	}
	return TRUE;
}
