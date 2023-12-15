#include "Logger.hpp"
#include "NewNameDialog.hpp"
#include "ProcessManager.hpp"
#include "TunnelsPage.hpp"

extern std::unique_ptr<Logger> gLogger;
extern std::unique_ptr<TunnelsManager> gTunnelsManager;
extern std::unique_ptr<ProcessManager> gProcessManager;

TunnelsPage::TunnelsPage()
	: editor_(MakeOne<TunnelEditor>())
{
	CtrlLayout(*this);

	{
		array_.AutoHideSb().HorzGrid(false).VertGrid(false).SetLineCy(Zx(26)); // 28 @100%

		array_.AddColumn(colId_, String::GetVoid()).HeaderTab().Hide();
		array_.AddColumn(colIcon_, String::GetVoid()).HeaderTab().Fixed(Zx(26)).SetMargin(0);
		array_.AddColumn(colName_, "Name");

		array_.WhenBar = [&](Bar& bar) { ShowMenu(bar); };
		array_.WhenSel = [&] {
			Id id{};
			auto i = array_.GetCursor();
			if(i >= 0) {
				id = array_.Get(i, colId_).ToString();
			}
			SetContent(id);
		};
		array_.WhenLeftDouble = [&] {
			Id id{};
			auto i = array_.GetCursor();
			if(i >= 0) {
				id = array_.Get(i, colId_).ToString();
			}

			if(id == gProcessManager->GetCurrentId()) {
				Disconnect();
			}
			else {
				Connect(id);
			}
		};

		ScanTunnels();
		Select(Id());
	}

	editor_->WhenRefresh = [&](Id& uuid) { SetContent(uuid); };

	gProcessManager->WhenStarted << [&](Id& id) { ScanTunnels(); };
	gProcessManager->WhenStopped << [&] { Disconnect(); };

	scroll_.scroll.y.AutoHide(false);
	scroll_.scroll.x.AutoHide(false).AutoDisable(false).Hide();
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

	cfg.Peers.Add(pick(TunnelPeer::GetDefault()));

	Add_(cfg);
}

void TunnelsPage::Add_(const TunnelConfig& config)
{
	if(gTunnelsManager->Add(config)) {
		ScanTunnels();
		Select(config.Interface.UUID);
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
		Select(Id());
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
	GuiLock __;

	Id idOld{};
	if(array_.IsCursor()) {
		idOld = array_.Get(array_.GetCursor(), colId_).ToString();
	}

	array_.Clear();

	auto current = gProcessManager->GetCurrentId();

	auto font = GetStdFont().Height(Zx(14));

	auto tunnels = gTunnelsManager->GetTunnels();
	for(const auto& item : ~(tunnels)) {
		auto& id = item.key;
		auto name = item.value.Interface.Name;
		auto img = id == current ? Image(Rescale(AppIcons::Connect(), Zx(19), Zx(19))) : Null;
		array_.Add(id.ToString(), img, AttrText(name).SetFont(font)); // 20 @100%
	}

	array_.RefreshLayoutDeep();

	if(!idOld.IsNull()) {
		Select(idOld);
	}
}

void TunnelsPage::Connect(const Id& uuid)
{
	auto i = array_.Find(uuid.ToString(), colId_);
	if(i < 0) {
		return;
	}

	if(!gProcessManager->Start(uuid)) {
		Exclamation("Could not start wiresock client");
		return;
	}

	ScanTunnels();
	Select(uuid);
}

void TunnelsPage::Disconnect()
{
	auto id = gProcessManager->GetCurrentId();
	if(!id.IsNull()) {
		auto i = array_.Find(id.ToString(), colId_);
		if(i < 0) {
			return;
		}

		if(!gProcessManager->Stop()) {
			Exclamation("Could not stop wiresock client");
			return;
		}
	}

	ScanTunnels();
}

void TunnelsPage::SetContent(const Id& uuid)
{
	GuiLock __;

	if(uuid.IsNull()) {
		scroll_.Hide();
		return;
	}

	scroll_.Show();
	scroll_.EnableScroll();

	editor_->SetId(uuid);

	scroll_.RemoveChild(editor_.Get());
	scroll_.Add(editor_->HSizePosZ());
	scroll_.AddPane(*editor_);
	scroll_.scroll.y.ScrollInto(0);
}

void TunnelsPage::Select(const Id& uuid)
{
	if(array_.GetCount() == 0) {
		return;
	}

	GuiLock __;

	if(!uuid.IsNull()) {
		auto str = uuid.ToString();
		for(int i = 0; i < array_.GetCount(); ++i) {
			if(array_.Get(i, colId_).ToString() == str) {
				array_.SetCursor(i);
				return;
			}
		}
	}

	array_.SetCursor(0);
}

void TunnelsPage::ShowMenu(Bar& bar)
{
	auto selected = array_.GetCursor() >= 0;
	String id = Null;
	if(selected) {
		id = array_.Get(array_.GetCursor(), colId_).ToString();
	}

	auto used = id == gProcessManager->GetCurrentId().ToString();
	auto connected = !gProcessManager->GetCurrentId().IsNull();

	int iconSize = Zx(15); // 16 @100%

	if(!connected) {
		bar.Add(selected, "Connect", Rescale(AppIcons::Connect(), iconSize, iconSize), [&, id] { Connect(id); });
	}
	else {
		bar.Add(connected, "Disconnect", Rescale(AppIcons::Disconnect(), iconSize, iconSize), [&] { Disconnect(); });
	}
	bar.Separator();
	bar.Add("Add", Rescale(AppIcons::Add(), iconSize, iconSize), [&] { Add(); });
	bar.Add("Import", Rescale(AppIcons::Folder(), iconSize, iconSize), [&] { Import(); });
	bar.Add(selected, "Duplicate", Rescale(AppIcons::Copy(), iconSize, iconSize), [&, id] { Duplicate(Id(id)); });
	bar.Add(selected & !used, "Rename", Rescale(AppIcons::Pen(), iconSize, iconSize), [&, id] { Rename(Id(id)); });
	bar.Add(selected & !used, "Delete", Rescale(AppIcons::Bin(), iconSize, iconSize), [&, id] { Delete(Id(id)); });
	bar.Separator();
	bar.Add(selected, "Add peer", Rescale(AppIcons::Add2(), iconSize, iconSize), [&, id] {
		editor_->AddPeer();
		scroll_.scroll.y.ScrollInto(scroll_.scroll.y.GetTotal());
	});
}
