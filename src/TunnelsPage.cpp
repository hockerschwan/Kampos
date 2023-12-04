#include "Logger.hpp"
#include "NewNameDialog.hpp"
#include "TunnelsPage.hpp"

extern std::unique_ptr<Logger> gLogger;
extern std::unique_ptr<TunnelsManager> gTunnelsManager;

TunnelsPage::TunnelsPage()
{
	CtrlLayout(*this);

	{
		array_.AddColumn(colId_, "UUID");
		array_.AddColumn(colName_, "Name");
		array_.HeaderTab(colId_).Hide();

		array_.NoAutoHideSb().HorzGrid(false).VertGrid(false).SetLineCy(Zy(21)); // 24 @100%
		array_.WhenBar = [&](Bar& bar) { ShowMenu(bar); };
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

	Add_(cfg);
}

void TunnelsPage::Add_(const TunnelConfig& config)
{
	if(gTunnelsManager->Add(config)) {
		ScanTunnels();
	}
	else {
		ErrorOK(String("Could not write to ") << UnixPath(Helper::TunnelsPath() << config.Interface.Name << ".conf"));
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

void TunnelsPage::Duplicate(const Id& uuid)
{
	auto tunnels = gTunnelsManager->GetTunnels();
	auto cfg = clone(tunnels.Get(uuid));
	cfg.Interface.UUID = Helper::GetNewUuid();

	Array<String> names{};
	FindFile find{};
	if(find.Search(Helper::TunnelsPath() << "*.conf")) {
		while(true) {
			auto n = ToLower(find.GetName());
			if(n.IsEmpty()) {
				break;
			}
			n.TrimEnd(".conf");
			names.Add(n);
			if(!find.Next()) {
				break;
			}
		}
	}

	auto m = String("Copy of ") << cfg.Interface.Name;
	auto f = false;
	for(const auto& name : names) {
		if(name == ToLower(m)) {
			f = true;
			break;
		}
	}

	if(f) {
		int i = 2;
		while(true) {
			auto m2 = m + "(" + IntStr(i) + ")";
			for(const auto& name : names) {
				if(name == ToLower(m2)) {
					f = false;
					break;
				}
			}
			if(f) {
				m = m2;
				break;
			}
			++i;
			f = true;
		}
	}

	cfg.Interface.Name = m;
	Add_(cfg);
}

void TunnelsPage::Import()
{
	FileSel sel{};
	sel.Multi().AllFilesType();
	if(sel.ExecuteOpen() != 1) {
		return;
	}

	Array<String> paths{};
	for(int i = 0; i < sel.GetCount(); ++i) {
		paths.Add(sel.GetFile(i));
	}

	if(gTunnelsManager->Import(paths)) {
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
		id = array_.Get(array_.GetCursor(), colId_).ToString();
	}

	int iconSize = Zx(15); // 16 @100%

	bar.Add("Add", Rescale(AppIcons::Add(), iconSize, iconSize), [&] { Add(); });
	bar.Add("Import", Rescale(AppIcons::Folder(), iconSize, iconSize), [&] { Import(); });
	bar.Separator();
	bar.Add(selected, "Duplicate", [&, id] { Duplicate(Id(id)); });
	bar.Add(selected, "Rename", Rescale(AppIcons::Pen(), iconSize, iconSize), [&, id] { Rename(Id(id)); });
	bar.Add(selected, "Delete", Rescale(AppIcons::Bin(), iconSize, iconSize), [&, id] { Delete(Id(id)); });
}
