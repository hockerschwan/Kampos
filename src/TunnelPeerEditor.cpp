#include "Logger.hpp"
#include "TunnelEditor.hpp"
#include "TunnelPeerEditor.hpp"

extern std::unique_ptr<Logger> gLogger;

TunnelPeerEditor::TunnelPeerEditor()
	: allowedIPs_(MakeOne<TunnelAddressEditor>())
	, disallowedIPs_(MakeOne<TunnelAddressEditor>())
	, allowedApps_(MakeOne<TunnelAppEditor>())
	, disallowedApps_(MakeOne<TunnelAppEditor>())
{
	CtrlLayout(*this);

	allowedIPs_->SetText("AllowedIPs");
	disallowedIPs_->SetText("DisallowedIPs");
	allowedApps_->SetText("AllowedApps");
	disallowedApps_->SetText("DisallowedApps");

	btnDelete_.SetImage(Rescale(AppIcons::Bin(), 16, 16)).Tip("Delete peer");

	grid_.SetColumns(2);
	grid_.Add(*allowedIPs_);
	grid_.Add(*disallowedIPs_);
	grid_.Add(*allowedApps_);
	grid_.Add(*disallowedApps_);

	editName_.WhenAction = [&] { Save(); };
	editEndpoint_.WhenAction = [&] { Save(); };
	spinKeepAlive_.WhenAction = [&] {
		if(StrIntValue(spinKeepAlive_.GetText().ToString()).IsNull()) {
			spinKeepAlive_.SetText("0");
		}
		Save();
	};
	editPresharedKey_.WhenAction = [&] { Save(); };
	editPublicKey_.WhenAction = [&] { Save(); };
	// todo: allowedIPs_->WhenAction = [&] { Save(); };
}

const TunnelPeer TunnelPeerEditor::Get() const
{
	TunnelPeer peer{};

	peer.Name = editName_.GetText().ToString();
	peer.AllowedApps = allowedApps_->Get();
	peer.DisallowedApps = disallowedApps_->Get();
	peer.AllowedIPs = allowedIPs_->Get();
	peer.DisallowedIPs = disallowedIPs_->Get();
	peer.Endpoint = editEndpoint_.GetText().ToString();
	peer.PersistentKeepalive = StrIntValue(spinKeepAlive_.GetText().ToString());
	peer.PresharedKey = editPresharedKey_.GetText().ToString();
	peer.PublicKey = editPublicKey_.GetText().ToString();

	return pick(peer);
}

void TunnelPeerEditor::Set(TunnelPeer& peer, int i)
{
	btnDelete_.WhenAction = [&, i] {
		String str{"Are you sure you want to delete $?"};
		auto name = editName_.GetText().ToString();
		if(name.IsEmpty()) {
			str.Replace("$", "a peer");
		}
		else {
			str.Replace("$", name);
		}

		if(PromptYesNo(str) == 1) {
			editor_->DeletePeer(i);
		}
	};

	editName_.SetText(peer.Name);
	editEndpoint_.SetText(peer.Endpoint);
	spinKeepAlive_.SetText(peer.PersistentKeepalive.ToString());
	editPublicKey_.SetText(peer.PublicKey);
	editPresharedKey_.SetText(peer.PresharedKey);

	allowedIPs_->Clear();
	for(auto& addr : peer.AllowedIPs) {
		allowedIPs_->Add(addr);
	}

	disallowedIPs_->Clear();
	for(auto& addr : peer.DisallowedIPs) {
		disallowedIPs_->Add(addr);
	}

	allowedApps_->Clear();
	for(auto& app : peer.AllowedApps) {
		allowedApps_->Add(app);
	}

	disallowedApps_->Clear();
	for(auto& app : peer.DisallowedApps) {
		disallowedApps_->Add(app);
	}
}

void TunnelPeerEditor::Save() { editor_->Save(); }
