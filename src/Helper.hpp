#ifndef _Helper_hpp_
#define _Helper_hpp_

#include <Core/Core.h>
#include <CtrlLib/CtrlLib.h>

using namespace Upp;

struct Helper {
public:
	// path
	static String LogsPath() { return RoamingPath() += "logs\\"; };
	static String RoamingPath() { return GetConfigFolder() + "\\"; };
	static String TunnelsPath() { return RoamingPath() += "tunnels\\"; };

	// ctrl
	static int CalcSize(const String& text, const Font& font, int minWidth = 0);

	// uuid
	static Id GetNewUuid() { return Id(ToLower(Uuid::Create().ToStringWithDashes())); };
	static Id GetVoidUuid() { return Id("00000000-0000-0000-0000-000000000000"); };

	// other
	static Tuple4<int, int, int, int> GetAppVersion();
	static const String GetTime();
	static const VectorMap<String, String> ForbiddenChars()
	{
		return {{"\\", ""}, {"/", ""}, {":", ""}, {"*", ""}, {"?", ""},
		        {"\"", ""}, {"<", ""}, {">", ""}, {"|", ""}};
	};
	static String FormatWithPrefix(IN int64 numBefore, OUT double& numAfter);
	static bool DoesArrayContainsNonEmptyString(const Array<String>& array);
};

#endif
