#ifndef _TunnelsManager_hpp_
#define _TunnelsManager_hpp_

#include "Helper.hpp"
#include "TunnelStructs.hpp"

struct TunnelsManager {
	typedef TunnelsManager CLASSNAME;

public:
	TunnelsManager();

	const ArrayMap<Id, TunnelConfig> GetTunnels() const;

	bool Add(const TunnelConfig& config);
	bool Delete(const Id& uuid);
	bool Rename(const Id& uuid, const String& name);

private:
	void ScanFiles();
	TunnelConfig Parse(const String& str);
	void Sort() { StableSortByValue(tunnels_); };

	ArrayMap<Id, TunnelConfig> tunnels_{};
};

#endif
