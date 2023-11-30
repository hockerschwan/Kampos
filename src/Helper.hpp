#ifndef _Helper_hpp_
#define _Helper_hpp_

#include <Core/Core.h>
#include <CtrlLib/CtrlLib.h>

using namespace Upp;

struct Helper {
	typedef Helper CLASSNAME;

public:
	// path
	static String LogsPath() { return RoamingPath() += "logs\\"; };
	static String RoamingPath() { return GetConfigFolder() + "\\"; };
	static String TunnelsPath() { return RoamingPath() += "tunnels\\"; };

	// ctrl
	static int CalcSize(const String& text, const Font& font, int minWidth = 0);

	// other
	static RGBA GetAppVersion();
	static const String GetTime();
};

#endif
