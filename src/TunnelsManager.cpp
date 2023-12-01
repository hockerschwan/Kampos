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

const String TunnelsManager::GetName(const Id& id) const
{
	auto pos = tunnels_.Find(id);
	if(pos < 0) {
		return Null;
	}
	return pick(clone(tunnels_[pos].Interface.Name));
}

void TunnelsManager::ScanFiles()
{
	FindFile find{};
	find.Search(Helper::TunnelsPath() + "*.conf");

	while(true) {
		FileIn st(Helper::TunnelsPath() + find.GetName());
		auto cfg = Parse(LoadStream(st));
		tunnels_.Add(Id(cfg.Interface.UUID), pick(cfg));

		if(!find.Next()) {
			break;
		}
	}
}

TunnelConfig TunnelsManager::Parse(const String& str)
{
	TunnelConfig cfg{};

	if(str.IsEmpty()) {
		return cfg;
	}

	String section{};
	int peerIndex = -1;
	for(String& line : Split(str, "\r\n")) {
		if(line.IsEmpty()) {
			continue;
		}

		// remove comment
		auto posComma = line.Find(";");
		if(posComma >= 0) {
			line = line.Left(posComma);
		}

		// trim spaces on both ends
		Helper::TrimWhiteSpaces(line);

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

		auto key = ToLower(Helper::TrimWhiteSpaces(line.Left(posEq)));
		auto value = Helper::TrimWhiteSpaces(line.Right(line.GetCount() - posEq - 1));

		if(section == "interface") {
			auto& ifc = cfg.Interface;

			if(key == "address") {
				for(auto& addr : Split(value, ",")) {
					addr = Helper::TrimWhiteSpaces(addr);
					// todo: check validity
					if(!addr.IsEmpty()) {
						ifc.Address.Add(addr);
					}
				}
			}
			else if(key == "dns") {
				for(auto& addr : Split(value, ",")) {
					addr = Helper::TrimWhiteSpaces(addr);
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
			else if(key == "#uuid") {
				if(value.IsEmpty() || value == Helper::GetVoidUuid()) {
					value = Helper::GetNewUuid();
				}
				ifc.UUID = Id(value);
			}
			else if(key == "#name") {
				ifc.Name = value;
			}
		}
		else if(section == "peer") {
			auto& peer = cfg.Peers[peerIndex];

			if(key == "allowedapps") {
				for(auto& app : Split(value, ",")) {
					app = Helper::TrimWhiteSpaces(app);
					if(!app.IsEmpty()) {
						peer.AllowedApps.Add(app);
					}
				}
			}
			else if(key == "allowedips") {
				for(auto& addr : Split(value, ",")) {
					addr = Helper::TrimWhiteSpaces(addr);
					if(!addr.IsEmpty()) {
						peer.AllowedIPs.Add(addr);
					}
				}
			}
			else if(key == "disallowedapps") {
				for(auto& app : Split(value, ",")) {
					app = Helper::TrimWhiteSpaces(app);
					if(!app.IsEmpty()) {
						peer.DisallowedApps.Add(app);
					}
				}
			}
			else if(key == "disallowedips") {
				for(auto& addr : Split(value, ",")) {
					addr = Helper::TrimWhiteSpaces(addr);
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
			else if(key == "#name") {
				peer.Name = value;
			}
		}
	}

	if(cfg.Interface.UUID == Helper::GetVoidUuid()) {
		cfg.Interface.UUID = Helper::GetNewUuid();
	}

	return cfg;
}

