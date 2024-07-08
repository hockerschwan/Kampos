#include "Helper.hpp"

int Helper::CalcSize(const String& text, const Font& font, int minWidth)
{
	int w = 0;
	for(const auto& c : text) {
		w += font.GetWidth(c);
	}
	return max(w, minWidth);
}

Tuple4<int, int, int, int> Helper::GetAppVersion()
{
	Tuple4<int, int, int, int> res{};
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

	res.Set(0, HIWORD(info->dwFileVersionMS));
	res.Set(1, LOWORD(info->dwFileVersionMS));
	res.Set(2, HIWORD(info->dwFileVersionLS));
	res.Set(3, LOWORD(info->dwFileVersionLS));
	return res;
}

const String Helper::GetTime()
{
	SYSTEMTIME t;
	GetLocalTime(&t);

	char buf[24] = {0};
	snprintf(buf, 24, "%04d-%02d-%02d %02d:%02d:%02d.%03d", t.wYear, t.wMonth, t.wDay, t.wHour,
	         t.wMinute, t.wSecond, t.wMilliseconds);

	return String(buf);
}

String Helper::FormatWithPrefix(int64 numBefore, double& numAfter)
{
	long double n = (long double)numBefore;
	String prefix{};
	auto l = log10(numBefore);

	if(l >= 18) {
		prefix = "E";
		n /= pow(10, 18);
	}
	else if(l >= 15) {
		prefix = "P";
		n /= pow(10, 15);
	}
	else if(l >= 12) {
		prefix = "T";
		n /= pow(10, 12);
	}
	else if(l >= 9) {
		prefix = "G";
		n /= pow(10, 9);
	}
	else if(l >= 6) {
		prefix = "M";
		n /= pow(10, 6);
	}
	else if(l >= 3) {
		prefix = "k";
		n /= pow(10, 3);
	}

	numAfter = n;
	return pick(prefix);
}

bool Helper::DoesArrayContainsNonEmptyString(const Array<String>& array)
{
	for(const auto& str : array) {
		if(!str.IsEmpty()) {
			return true;
		}
	}

	return false;
}
