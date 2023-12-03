#include "Logger.hpp"
#include "TunnelsManager.hpp"

extern std::unique_ptr<Logger> gLogger;

TunnelsManager::TunnelsManager()
{
	RealizeDirectory(Helper::TunnelsPath());
	ScanFiles();
}

const ArrayMap<Id, TunnelConfig> TunnelsManager::GetTunnels() const
{
	ArrayMap<Id, TunnelConfig> cl{};
	for(const auto& item : ~(tunnels_)) {
		cl.Add(pick(clone(item.key)), pick(clone(item.value)));
	}
	return pick(cl);
}

bool TunnelsManager::Add(const TunnelConfig& config)
{
	if(!SaveFile(Helper::TunnelsPath() << config.Interface.Name << ".conf", config.ToString())) {
		gLogger->Log(String("AddTunnel failed: ") << config.Interface.Name);
		return false;
	}

	ScanFiles();
	return true;
}

bool TunnelsManager::Delete(const Id& uuid)
{
	// todo: do not delete currently used tunnel

	auto& cfg = tunnels_.Get(uuid);
	auto path = Helper::TunnelsPath() << cfg.Interface.Name << ".conf";
	if(!FileExists(path)) {
		gLogger->Log(String("Delete failed: ") << path << " does not exist.");
		return false;
	}

	if(!FileDelete(path)) {
		gLogger->Log(String("Delete failed: Could not delete ") << path);
		return false;
	}

	ScanFiles();
	return true;
}

bool TunnelsManager::Rename(const Id& uuid, const String& name)
{
	auto& cfg = tunnels_.Get(uuid);
	auto oldName = cfg.Interface.Name;
	auto oldPath = Helper::TunnelsPath() << oldName << ".conf";
	if(!FileExists(oldPath)) {
		gLogger->Log(String("Rename failed: ") << oldPath << " does not exist.");
		return false;
	}

	cfg.Interface.Name = name;
	auto path = Helper::TunnelsPath() << name << ".conf";
	if(FileExists(path)) {
		cfg.Interface.Name = oldName;
		gLogger->Log(String("Rename failed: ") << path << " already exists.");
		return false;
	}

	if(!FileDelete(oldPath)) {
		cfg.Interface.Name = oldName;
		gLogger->Log(String("Rename failed: Could not delete ") << oldPath);
		return false;
	}

	auto res = SaveFile(path, cfg.ToString());
	if(res) {
		ScanFiles();
	}
	else {
		cfg.Interface.Name = oldName;
		gLogger->Log(String("Rename failed: Could not save ") << path);
	}
	return res;
}

void TunnelsManager::ScanFiles()
{
	tunnels_.Clear();

	FindFile find{};
	find.Search(Helper::TunnelsPath() << "*.conf");

	while(true) {
		FileIn st(Helper::TunnelsPath() << find.GetName());
		auto cfg = Parse(LoadStream(st));
		tunnels_.Add(Id(cfg.Interface.UUID), pick(cfg));

		if(!find.Next()) {
			break;
		}
	}

	Sort();
}

TunnelConfig TunnelsManager::Parse(const String& str)
{
	TunnelConfig cfg{};

	if(str.IsEmpty()) {
		return cfg;
	}

	String section{};
	int peerIndex = -1;
	for(String& line : Split(str, "\n")) {
		line.Replace("\r", "");
		if(line.IsEmpty()) {
			continue;
		}

		// remove comment
		auto posComma = line.Find(";");
		if(posComma >= 0) {
			line = line.Left(posComma);
		}

		// trim spaces on both ends
		line.TrimStart("#");
		line = TrimBoth(line);

		// new section starts
		if(line.StartsWith("[") && line.EndsWith("]")) {
			section = ToLower(line.Left(line.GetCount() - 1).Right(line.GetCount() - 2));
			if(section == "peer") {
				++peerIndex;
				cfg.Peers.Add(TunnelPeer());
			}
			continue;
		}

		auto posEq = line.FindFirstOf("=");
		if(posEq < 0) {
			continue;
		}

		auto key = ToLower(TrimBoth(line.Left(posEq)));
		auto value = TrimBoth(line.Right(line.GetCount() - posEq - 1));

		if(section == "interface") {
			auto& ifc = cfg.Interface;

			if(key == "address") {
				for(auto& addr : Split(value, ",")) {
					addr = TrimBoth(addr);
					// todo: check validity
					if(!addr.IsEmpty()) {
						ifc.Address.Add(addr);
					}
				}
			}
			else if(key == "dns") {
				for(auto& addr : Split(value, ",")) {
					addr = TrimBoth(addr);
					if(!addr.IsEmpty()) {
						ifc.DNS.Add(addr);
					}
				}
			}
			else if(key == "mtu") {
				Value mtu = StrInt(value);
				if(!mtu.IsNull()) {
					ifc.MTU = mtu;
				}
			}
			else if(key == "privatekey") {
				ifc.PrivateKey = value;
			}
			else if(key == "uuid") {
				if(value.IsEmpty() || value == Helper::GetVoidUuid()) {
					value = Helper::GetNewUuid();
				}
				ifc.UUID = Id(value);
			}
			else if(key == "name") {
				ifc.Name = value;
			}
		}
		else if(section == "peer") {
			auto& peer = cfg.Peers[peerIndex];

			if(key == "allowedapps") {
				for(auto& app : Split(value, ",")) {
					app = TrimBoth(app);
					if(!app.IsEmpty()) {
						peer.AllowedApps.Add(app);
					}
				}
			}
			else if(key == "allowedips") {
				for(auto& addr : Split(value, ",")) {
					addr = TrimBoth(addr);
					if(!addr.IsEmpty()) {
						peer.AllowedIPs.Add(addr);
					}
				}
			}
			else if(key == "disallowedapps") {
				for(auto& app : Split(value, ",")) {
					app = TrimBoth(app);
					if(!app.IsEmpty()) {
						peer.DisallowedApps.Add(app);
					}
				}
			}
			else if(key == "disallowedips") {
				for(auto& addr : Split(value, ",")) {
					addr = TrimBoth(addr);
					if(!addr.IsEmpty()) {
						peer.DisallowedIPs.Add(addr);
					}
				}
			}
			else if(key == "endpoint") {
				peer.Endpoint = value;
			}
			else if(key == "persistentkeepalive") {
				Value pka = StrInt(value);
				if(!pka.IsNull()) {
					peer.PersistentKeepalive = pka;
				}
			}
			else if(key == "presharedkey") {
				peer.PresharedKey = value;
			}
			else if(key == "publickey") {
				peer.PublicKey = value;
			}
			else if(key == "name") {
				peer.Name = value;
			}
		}
	}

	if(cfg.Interface.UUID == Helper::GetVoidUuid()) {
		cfg.Interface.UUID = Helper::GetNewUuid();
	}

	return cfg;
}
