#ifndef _Helper_hpp_
#define _Helper_hpp_

#include <Core/Core.h>
#include <CtrlLib/CtrlLib.h>

#define UUID_SYSTEM_GENERATOR
#include <stduuid/uuid.h>

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

	// string
	static String TrimWhiteSpaces(const String& str);

	// uuid
	static Id GetNewUuid() { return Id(uuids::to_string(uuids::uuid_system_generator{}())); };
	static Id GetVoidUuid() { return Id(uuids::to_string(uuids::uuid())); };

	// other
	static RGBA GetAppVersion();
	static const String GetTime();
};

#endif
