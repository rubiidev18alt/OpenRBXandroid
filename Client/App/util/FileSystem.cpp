#include "util/FileSystem.h"

#ifdef _WIN32
#include <shlobj.h>
#include <windows.h>
#else
#include <cstdlib>
#include <filesystem>
#include <system_error>
#endif

namespace RBX {

#ifdef _WIN32

class VistaAPIs
{
	using GetKnownFolderPathPtr = HRESULT(WINAPI*)(const GUID& rfid, DWORD dwFlags, HANDLE hToken, PWSTR* ppszPath);

	GetKnownFolderPathPtr getKnownFolderPath;
	HINSTANCE gShell32DLLInst;

public:
	VistaAPIs() : getKnownFolderPath(nullptr)
	{
		gShell32DLLInst = LoadLibrary("Shell32.dll");
		if (gShell32DLLInst) {
			getKnownFolderPath = (GetKnownFolderPathPtr) GetProcAddress(gShell32DLLInst, "SHGetKnownFolderPath");
		}
	}

	~VistaAPIs();

	HRESULT SHGetKnownFolderPath(const GUID& rfid, DWORD dwFlags, HANDLE hToken, PWSTR* ppszPath)
	{
		if (getKnownFolderPath) {
			return getKnownFolderPath(rfid, dwFlags, hToken, ppszPath);
		}
		return E_NOTIMPL;
	}

	struct TOKEN_MANDATORY_LABEL
	{
		SID_AND_ATTRIBUTES Label;
	};

	using PTOKEN_MANDATORY_LABEL = TOKEN_MANDATORY_LABEL*;

	HRESULT STDMETHODCALLTYPE GetIntegrity(BYTE* pVal);
};

VistaAPIs::~VistaAPIs()
{
	if (gShell32DLLInst) {
		FreeLibrary(gShell32DLLInst);
	}
}

HRESULT STDMETHODCALLTYPE VistaAPIs::GetIntegrity(BYTE* pVal)
{
	*pVal = 0;

	OSVERSIONINFO osvi = {0};
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osvi);
	if (osvi.dwMajorVersion < 6) {
		return S_OK;
	}

	HANDLE hToken;
	HANDLE hProcess = GetCurrentProcess();
	if (OpenProcessToken(hProcess, TOKEN_QUERY | TOKEN_QUERY_SOURCE, &hToken)) {
		DWORD dwLengthNeeded;
		if (!GetTokenInformation(hToken, (TOKEN_INFORMATION_CLASS) 25, nullptr, 0, &dwLengthNeeded)) {
			DWORD dwError = GetLastError();
			if (dwError == ERROR_INSUFFICIENT_BUFFER) {
				PTOKEN_MANDATORY_LABEL pTIL = (PTOKEN_MANDATORY_LABEL) LocalAlloc(0, dwLengthNeeded);
				if (pTIL != nullptr) {
					if (GetTokenInformation(
							hToken, (TOKEN_INFORMATION_CLASS) 25, pTIL, dwLengthNeeded, &dwLengthNeeded
						)) {
						DWORD dwIntegrityLevel = *GetSidSubAuthority(
							pTIL->Label.Sid, (DWORD) (UCHAR) (*GetSidSubAuthorityCount(pTIL->Label.Sid) - 1)
						);

						if (dwIntegrityLevel < 0x2000) {
							*pVal = 1;
						}
						else if (dwIntegrityLevel < 0x3000) {
							*pVal = 2;
						}
						else {
							*pVal = 3;
						}
					}
					LocalFree(pTIL);
				}
			}
		}
		CloseHandle(hToken);
	}

	return S_OK;
}

static std::string narrow(const wchar_t* text)
{
	int length = WideCharToMultiByte(CP_ACP, 0, text, -1, nullptr, 0, nullptr, nullptr);
	std::string result(length > 0 ? length - 1 : 0, 0);
	if (length > 1) {
		WideCharToMultiByte(CP_ACP, 0, text, -1, result.data(), length, nullptr, nullptr);
	}
	return result;
}

static std::string withBackslash(std::string path)
{
	if (!path.empty() && path.back() != '\\') {
		path += '\\';
	}
	return path;
}

std::string FileSystem::getUserAppDataDirectory(bool create, const char* subDirectory)
{
	VistaAPIs apis;

	BYTE integrity;
	apis.GetIntegrity(&integrity);
	if (integrity > 0) {
		GUID folderID;
		if (integrity == 1) {
			CLSIDFromString(L"{A520A1A4-1780-4FF6-BD18-167343C5AF16}", &folderID);
		}
		else {
			CLSIDFromString(L"{F1B32785-6FBA-4FCF-9D55-7B8E7F157091}", &folderID);
		}

		LPWSTR lppwstrPath = nullptr;
		if (apis.SHGetKnownFolderPath(folderID, create ? 0x8000 : 0, nullptr, &lppwstrPath) != E_NOTIMPL) {
			std::string s = narrow(lppwstrPath);
			CoTaskMemFree(lppwstrPath);

			s += "\\Roblox\\";
			if (create) {
				if (!CreateDirectory(s.c_str(), nullptr) && GetLastError() != ERROR_ALREADY_EXISTS) {
					return "";
				}
			}

			if (subDirectory) {
				s += subDirectory;
				if (create) {
					if (!CreateDirectory(s.c_str(), nullptr) && GetLastError() != ERROR_ALREADY_EXISTS) {
						return "";
					}
				}
			}

			return withBackslash(s);
		}
	}

	int flags = create ? CSIDL_FLAG_CREATE : 0;
	std::string s = "Roblox\\";
	if (subDirectory) {
		s += subDirectory;
	}

	char path[MAX_PATH];
	HRESULT hr = SHGetFolderPathAndSubDir(nullptr, flags | CSIDL_APPDATA, nullptr, 0, s.c_str(), path);
	if (hr != S_OK) {
		hr = SHGetFolderPathAndSubDir(nullptr, flags | CSIDL_COMMON_APPDATA, nullptr, 0, s.c_str(), path);
	}
	if (hr == S_OK) {
		return withBackslash(path);
	}
	else {
		return "";
	}
}

std::string FileSystem::getCacheDirectory(bool create)
{
	VistaAPIs apis;

	BYTE integrity;
	apis.GetIntegrity(&integrity);
	if (integrity == 1) {
		GUID folderID;
		CLSIDFromString(L"{352481E8-33BE-4251-BA85-6007CAEDCF9D", &folderID);

		LPWSTR lppwstrPath = nullptr;
		if (apis.SHGetKnownFolderPath(folderID, 0x8000, nullptr, &lppwstrPath) != E_NOTIMPL) {
			std::string s = narrow(lppwstrPath);
			CoTaskMemFree(lppwstrPath);

			s += "\\Roblox\\";
			if (create) {
				if (!CreateDirectory(s.c_str(), nullptr) && GetLastError() != ERROR_ALREADY_EXISTS) {
					return "";
				}
			}

			return withBackslash(s);
		}
	}

	char path[MAX_PATH];
	if (SHGetFolderPathAndSubDir(
			nullptr, (create ? CSIDL_FLAG_CREATE : 0) | CSIDL_COMMON_APPDATA, nullptr, 0, "Roblox\\Cache", path
		) == S_OK) {
		return path;
	}
	else {
		return "";
	}
}

std::string FileSystem::getContentDirectory()
{
	char path[MAX_PATH];
	if (SHGetFolderPathAndSubDir(nullptr, CSIDL_COMMON_APPDATA, nullptr, 0, "Roblox\\content\\", path) == S_OK) {
		return path;
	}
	else {
		return "";
	}
}

#else

static std::string sAndroidFilesDir;
static std::string sAndroidCacheDir;
static std::string sAndroidContentDir;

static std::string ensureTrailingSlash(std::string path)
{
	if (!path.empty() && path.back() != '/') {
		path += '/';
	}
	return path;
}

void FileSystem::setAndroidBasePaths(
	const std::string& filesDir, const std::string& cacheDir, const std::string& contentDir
)
{
	sAndroidFilesDir = ensureTrailingSlash(filesDir);
	sAndroidCacheDir = ensureTrailingSlash(cacheDir);
	sAndroidContentDir = ensureTrailingSlash(contentDir);
}

static std::string getBaseDirectory(const char* variable, const char* homeSubDirectory)
{
	const char* base = std::getenv(variable);
	if (base && *base) {
		return base;
	}

	const char* home = std::getenv("HOME");
	if (home && *home) {
		return std::string(home) + homeSubDirectory;
	}

	return "";
}

static bool createDirectory(const std::string& path)
{
	std::error_code error;
	std::filesystem::create_directories(path, error);
	return !error;
}

std::string FileSystem::getUserAppDataDirectory(bool create, const char* subDirectory)
{
	std::string path;
	if (!sAndroidFilesDir.empty()) {
		path = sAndroidFilesDir + "Roblox/";
	}
	else {
		std::string base = getBaseDirectory("XDG_DATA_HOME", "/.local/share");
		if (base.empty()) {
			return "";
		}
		path = base + "/Roblox/";
	}

	if (subDirectory) {
		path += subDirectory;
		path += '/';
	}

	if (create && !createDirectory(path)) {
		return "";
	}

	return path;
}

std::string FileSystem::getCacheDirectory(bool create)
{
	std::string path;
	if (!sAndroidCacheDir.empty()) {
		path = sAndroidCacheDir + "Roblox/";
	}
	else {
		std::string base = getBaseDirectory("XDG_CACHE_HOME", "/.cache");
		if (base.empty()) {
			return "";
		}
		path = base + "/Roblox/";
	}

	if (create && !createDirectory(path)) {
		return "";
	}

	return path;
}

std::string FileSystem::getContentDirectory()
{
	if (!sAndroidContentDir.empty()) {
		std::error_code error;
		if (std::filesystem::is_directory(sAndroidContentDir, error)) {
			return sAndroidContentDir;
		}
	}

	std::string base = getBaseDirectory("XDG_DATA_HOME", "/.local/share");
	if (base.empty()) {
		return "";
	}

	std::string path = base + "/Roblox/content/";
	std::error_code error;
	if (!std::filesystem::is_directory(path, error)) {
		return "";
	}

	return path;
}

#endif

#ifdef _WIN32
void FileSystem::setAndroidBasePaths(const std::string&, const std::string&, const std::string&)
{
}
#endif

} // namespace RBX
