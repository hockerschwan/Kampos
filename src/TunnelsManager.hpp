#ifndef _TunnelsManager_hpp_
#define _TunnelsManager_hpp_

#include "Helper.hpp"
#include "TunnelStructs.hpp"

struct TunnelsManager {
public:
	TunnelsManager();
	~TunnelsManager() { ClearEvents(); };

	const ArrayMap<Id, TunnelConfig> GetTunnels() const;
	bool GetConfig(const Id& uuid, TunnelConfig& out) const;

	bool Add(const TunnelConfig& config);
	bool Delete(const Id& uuid);
	bool Import(const Array<String>& paths);
	bool Rename(const Id& uuid, const String& name);

	bool Save(const Id& uuid, const TunnelConfig& config, bool scan = false);

	void ClearEvents() { WhenListChanged.Clear(); };

	Event<> WhenListChanged;

private:
	void ScanFiles();
	TunnelConfig Parse(const String& str) const;
	void Sort();

	ArrayMap<Id, TunnelConfig> tunnels_{};

	Mutex mutex_{};
};

#endif
