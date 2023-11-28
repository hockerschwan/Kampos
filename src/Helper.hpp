#ifndef _Helper_hpp_
#define _Helper_hpp_

#include <Core/Core.h>

using namespace Upp;

class Helper {
	typedef Helper CLASSNAME;

public:
	// path
	static String AppPath() { return GetExeFilePath(); };
	static String LogsPath() { return RoamingPath() += "logs\\"; };
	static String RoamingPath() { return GetConfigFolder() + "\\"; };
	static String TunnelsPath() { return RoamingPath() += "tunnels\\"; };
};

#endif
