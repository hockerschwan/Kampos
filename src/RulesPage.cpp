#include "Logger.hpp"
#include "RuleManager.hpp"
#include "RulesPage.hpp"

extern std::unique_ptr<Logger> gLogger;
extern std::unique_ptr<RuleManager> gRuleManager;

const char* RuleDnDName = "RuleDragAndDrop";

RulesPage::RulesPage()
	: editor_(MakeOne<RuleEditor>())
{
	CtrlLayout(*this);

	content_.Hide();

	{
		array_.AutoHideSb().HorzGrid(false).VertGrid(false).SetLineCy(Zx(26)); // 28 @100%

		array_.AddColumn(colId_, String::GetVoid()).HeaderTab().Hide();
		array_.AddColumn(colIcon_, String::GetVoid())
			.With([](One<Ctrl>& x) {
				auto& img = x.Create<ImageCtrl>();
				img.SetImage(Rescale(AppIcons::Check(), Zx(19), Zx(19))).Hide();
			})
			.HeaderTab()
			.Fixed(Zx(26))
			.SetMargin(0);
		array_.AddColumn(colName_, "Name");

		array_.WhenBar = [&](Bar& bar) { ShowMenu(bar); };
		array_.WhenDrag = [&] { Drag(); };
		array_.WhenDropInsert = [&](int i, PasteClip& clip) { DropInsert(i, clip); };
		array_.WhenSel = [&] {
			Id id{};
			auto i = array_.GetCursor();
			if(i >= 0) {
				id = array_.Get(i, colId_).ToString();
			}
			SetContent(id);
		};

		ScanRules();
		Select(Id());
	}

	content_.Add(editor_->SizePos());
	editor_->WhenAction = [&] {
		Save();
		gRuleManager->CheckConditions();
	};
	editor_->WhenNameChanged = [&](int l, int h) {
		Save();
		ScanRules();
		editor_->editName_.SetSelection(l, h);
	};

	gRuleManager->WhenRuleChanged << [&](Id uuid) { RefreshImage(); };
}

void RulesPage::SetContent(const Id& uuid)
{
	GuiLock __;

	if(uuid.IsNull()) {
		content_.Hide();
		return;
	}

	editor_->SetId(uuid);
	content_.Show();
}

void RulesPage::RefreshImage()
{
	auto current = gRuleManager->GetCurrentId();
	for(int i = 0; i < array_.GetCount(); ++i) {
		auto id = array_.Get(i, colId_).ToString();
		auto* pImg = (ImageCtrl*)array_.GetCtrl(i, colIcon_);
		if(pImg) {
			pImg->Show(current.ToString() == id);
		}
	}
}

void RulesPage::Save()
{
	auto rule = editor_->Get();
	gRuleManager->Save(pick(rule));
}

void RulesPage::ScanRules()
{
	GuiLock __;

	Id idOld{};
	if(array_.IsCursor()) {
		idOld = array_.Get(array_.GetCursor(), colId_).ToString();
	}

	array_.Clear();

	auto current = gRuleManager->GetCurrentId();

	auto rules = gRuleManager->GetRules();
	for(const auto& item : ~(rules)) {
		auto& id = item.key;
		auto name = item.value.Name;
		array_.Add(id.ToString(), Null, AttrText(name).SetFont(StdFontZ(14)));
	}

	RefreshImage();
	array_.RefreshLayoutDeep();

	if(!idOld.IsNull()) {
		Select(idOld);
	}
}

void RulesPage::Select(const Id& uuid)
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

void RulesPage::ShowMenu(Bar& bar)
{
	auto selected = array_.GetCursor() >= 0;
	String id = Null;
	if(selected) {
		id = array_.Get(array_.GetCursor(), colId_).ToString();
	}

	int iconSize = Zx(15); // 16 @100%

	bar.Add("Add", Rescale(AppIcons::Add(), iconSize, iconSize), [&] {
		auto uuid = gRuleManager->Add();
		ScanRules();
		Select(uuid);
	});
	bar.Add(selected, "Delete", Rescale(AppIcons::Bin(), iconSize, iconSize), [&, id] {
		Rule rule{};
		if(!gRuleManager->GetRule(id, rule)) {
			return;
		}

		if(PromptYesNo(String("Are you sure you want to delete ") << rule.Name << "?") == 1) {
			array_.RemoveSelection();
			gRuleManager->Remove(Id(id));
			ScanRules();
			array_.GoBegin();
		}
	});
}

void RulesPage::Drag()
{
	if(!array_.IsCursor()) {
		return;
	}

	auto i = array_.GetCursor();
	String id = array_.Get(i, colId_).ToString();

	auto clip = InternalClip(array_, RuleDnDName);
	Append(clip, id);

	array_.DoDragAndDrop(clip);
}

void RulesPage::DropInsert(int i, PasteClip& clip)
{
	if(AcceptInternal<ArrayCtrl>(clip, RuleDnDName)) {
		array_.InsertDrop(i, clip);
		array_.SetFocus();

		auto id = GetString(clip);
		gRuleManager->Move(Id(id), i);
	}
}
