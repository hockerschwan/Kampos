#include "Logger.hpp"
#include "NewNameDialog.hpp"
#include "TunnelsManager.hpp"
#include "TunnelsPage.hpp"

extern std::unique_ptr<Logger> gLogger;
extern std::unique_ptr<TunnelsManager> gTunnelsManager;

TunnelsPage::TunnelsPage()
{
	CtrlLayout(*this);

	{
		array_.AddColumn("UUID");
		array_.AddColumn("Name");
		array_.HeaderTab(0).Hide();

		array_.NoAutoHideSb().HorzGrid(false).VertGrid(false).SetLineCy(Zy(21)); // 24 @100%
		array_.WhenBar = THISBACK(ShowMenu);
		ScanTunnels();
		if(array_.GetCount() > 0) {
			array_.SetCursor(0);
		}
	}
}

void TunnelsPage::Add()
{
	NewNameDialog dlg;
	if(dlg.Execute() != IDOK) {
		return;
	}

	auto name = (~dlg.text).ToString();

	TunnelConfig cfg{};
	cfg.Interface.MTU = 1400;
	cfg.Interface.Name = name;
	cfg.Interface.UUID = Helper::GetNewUuid();

	TunnelPeer peer{};
	peer.AllowedIPs.Add("0.0.0.0/0");
	peer.AllowedIPs.Add("::/0");
	peer.DisallowedIPs.Add("192.168.0.0/16");
	peer.DisallowedIPs.Add("10.0.0.0/8");
	peer.DisallowedIPs.Add("172.16.0.0/12");
	peer.PersistentKeepalive = 30;
	cfg.Peers.Add(pick(peer));

	if(gTunnelsManager->Add(cfg)) {
		ScanTunnels();
	}
	else {
		ErrorOK(String("Could not write to ") << UnixPath(Helper::TunnelsPath() << name << ".conf"));
	}
}

void TunnelsPage::Delete(const Id& uuid)
{
	auto tunnels = gTunnelsManager->GetTunnels();
	auto& cfg = tunnels.Get(uuid);
	auto oldName = cfg.Interface.Name;

	if(PromptYesNo(String("Are you sure you want to delete ") << oldName << "?") == 1) {
		gTunnelsManager->Delete(uuid);
		ScanTunnels();
	}
}

void TunnelsPage::Rename(const Id& uuid)
{
	auto tunnels = gTunnelsManager->GetTunnels();
	auto& cfg = tunnels.Get(uuid);
	auto oldName = cfg.Interface.Name;

	NewNameDialog dlg;
	dlg.text.SetText(oldName);
	if(dlg.Execute() != IDOK) {
		return;
	}

	auto name = (~dlg.text).ToString();
	if(gTunnelsManager->Rename(uuid, name)) {
		ScanTunnels();
	}
	else {
		ErrorOK(String("Failed to rename ") << UnixPath(Helper::TunnelsPath() << oldName << ".conf"));
	};
}

void TunnelsPage::ScanTunnels()
{
	array_.Clear();

	auto tunnels = gTunnelsManager->GetTunnels();
	for(const auto& item : ~(tunnels)) {
		auto& id = item.key;
		auto name = item.value.Interface.Name;
		array_.Add(id.ToString(), name, true);
	}
}

void TunnelsPage::ShowMenu(Bar& bar)
{
	auto selected = array_.GetCursor() >= 0;
	String id = Null;
	if(selected) {
		id = array_.Get(array_.GetCursor(), 0);
	}

	int iconSize = Zx(15); // 16 @100%

	bar.Add("Add", Rescale(AppIcons::Add(), iconSize, iconSize), THISBACK(Add));
	bar.Add(selected, "Delete", Rescale(AppIcons::Bin(), iconSize, iconSize), THISBACK1(Delete, Id(id)));
	bar.Separator();
	bar.Add(selected, "Rename", Rescale(AppIcons::Pen(), iconSize, iconSize), THISBACK1(Rename, Id(id)));
}
