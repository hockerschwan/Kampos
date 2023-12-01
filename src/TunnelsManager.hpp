#ifndef _TunnelsManager_hpp_
#define _TunnelsManager_hpp_

#include "Helper.hpp"
#include "TunnelStructs.hpp"

struct TunnelsManager {
	typedef TunnelsManager CLASSNAME;

public:
	TunnelsManager();

	const ArrayMap<Id, TunnelConfig> GetTunnels() const;
	const String GetName(const Id& id) const;

private:
	void ScanFiles();
	TunnelConfig Parse(const String& str);

	ArrayMap<Id, TunnelConfig> tunnels_{};
};

#endif
