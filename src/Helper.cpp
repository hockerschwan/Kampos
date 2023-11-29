#include "Helper.hpp"

int Helper::CalcSize(const String& text, const Font& font, int minWidth)
{
	int w = 0;
	for(const auto& c : text) {
		w += font.GetWidth(c);
	}
	return max(w, minWidth);
}

RGBA Helper::GetAppVersion()
{
	auto res = RGBA();
	auto path = GetExeFilePath().ToWString().ToStd();

	DWORD _{};
	auto size = GetFileVersionInfoSizeW(path.c_str(), 0);

	TCHAR buf[size];
	if(GetFileVersionInfoW(path.c_str(), 0, size + 0, &buf) == FALSE) {
		return res;
	}

	VS_FIXEDFILEINFO* info = nullptr;
	UINT len{};
	if(VerQueryValueW(&buf, L"\\", (LPVOID*)&info, &len) == FALSE) {
		return res;
	}

	res.r = HIWORD(info->dwFileVersionMS);
	res.g = LOWORD(info->dwFileVersionMS);
	res.b = HIWORD(info->dwFileVersionLS);
	res.a = LOWORD(info->dwFileVersionLS);
	return res;
}
