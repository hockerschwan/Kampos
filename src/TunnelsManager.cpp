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
		cl.Add(clone(item.key), clone(item.value));
	}
	return pick(cl);
}

bool TunnelsManager::GetConfig(const Id& uuid, TunnelConfig& out) const
{
	auto i = tunnels_.Find(uuid);
	if(i < 0) {
		return false;
	}

	out = pick(clone(tunnels_.Get(uuid)));
	return true;
}

bool TunnelsManager::Add(const TunnelConfig& config)
{
	if(!SaveFile(Helper::TunnelsPath() << config.Interface.Name << ".conf", config.ToString())) {
		gLogger->Log(String("AddTunnel failed: ") << config.Interface.Name);
		return false;
	}

	ScanFiles();
	WhenListChanged();
	return true;
}

bool TunnelsManager::Delete(const Id& uuid)
{
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
	WhenListChanged();
	return true;
}

bool TunnelsManager::Import(const Array<String>& paths)
{
	auto res = false;

	for(const auto& path : paths) {
		auto cfg = Parse(LoadFile(path));

		if(cfg == TunnelConfig::GetVoid()) {
			gLogger->Log(String("Import failed: Could not parse " << path));
			continue;
		}

		if(cfg.Interface.UUID == Helper::GetVoidUuid()) {
			cfg.Interface.UUID = Helper::GetNewUuid();
		}

		if(cfg.Interface.Name.IsEmpty()) {
			auto t = TimeFromUTC(
				std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count());

			auto name = GetFileName(path);
			name.TrimEnd(GetFileExtPos(path));
			name << "_" << FormatTime(t, "YYYYMMDD-HHmmss");
			name = Replace(name, Helper::ForbiddenChars());

			cfg.Interface.Name = name;
		}

		auto savePath = Helper::TunnelsPath() << cfg.Interface.Name << ".conf";
		if(!SaveFile(savePath, cfg.ToString())) {
			gLogger->Log(String("Import failed: ") << savePath);
			continue;
		}

		gLogger->Log(String("Imported: ") << path << " as " << savePath);
		res = true;
	}

	if(res) {
		ScanFiles();
		WhenListChanged();
	}

	return res;
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
		WhenListChanged();
	}
	else {
		cfg.Interface.Name = oldName;
		gLogger->Log(String("Rename failed: Could not save ") << path);
	}
	return res;
}

bool TunnelsManager::Save(const Id& uuid, const TunnelConfig& config, bool scan)
{
	mutex_.Enter();

	if(tunnels_.Find(uuid) > 0) {
		auto& cfg = tunnels_.Get(uuid);
		cfg = clone(config);
	}

	auto res = SaveFile(Helper::TunnelsPath() << config.Interface.Name << ".conf", config.ToString());

	mutex_.Leave();

	if(scan) {
		ScanFiles();
	}

	return res;
};

void TunnelsManager::ScanFiles()
{
	tunnels_.Clear();

	FindFile find{};
	find.Search(Helper::TunnelsPath() << "*.conf");

	while(true) {
		auto path = Helper::TunnelsPath() << find.GetName();
		FileIn st(path);

		auto name = find.GetName();
		name.TrimEnd(".conf");

		auto cfg = Parse(LoadStream(st));
		st.Close();

		if(cfg != TunnelConfig::GetVoid()) {
			if(cfg.Interface.Name.IsEmpty()) {
				cfg.Interface.Name = name;
			}

			if(cfg.Interface.UUID == Helper::GetVoidUuid()) {
				Sys(String("attrib -r " << path)); // remove read only flag
				FileDelete(path);

				cfg.Interface.UUID = Helper::GetNewUuid();
				Save(cfg.Interface.UUID, cfg);
			}

			tunnels_.Add(Id(cfg.Interface.UUID), pick(cfg));
		}

		if(!find.Next()) {
			break;
		}
	}

	Sort();
}

TunnelConfig TunnelsManager::Parse(const String& str) const
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
			else if(key == "preup") {
				ifc.PreUp.Add(value);
			}
			else if(key == "postup") {
				ifc.PostUp.Add(value);
			}
			else if(key == "predown") {
				ifc.PreDown.Add(value);
			}
			else if(key == "postdown") {
				ifc.PostDown.Add(value);
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

	return cfg;
}
