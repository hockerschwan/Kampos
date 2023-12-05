#include "TunnelPeerEditor.hpp"

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
}

void TunnelPeerEditor::Set(TunnelPeer& peer)
{
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
