#include "Logger.hpp"
#include "RuleEditor.hpp"
#include "TunnelsManager.hpp"

extern std::unique_ptr<Logger> gLogger;
extern std::unique_ptr<TunnelsManager> gTunnelsManager;
extern std::unique_ptr<RuleManager> gRuleManager;

void RuleConditions::AddCondition(const Any& any)
{
	int i = editors_.GetCount();

	if(any.Is<RuleConditionSSID>()) {
		auto cond = any.Get<RuleConditionSSID>();

		Any editorAny{};
		editorAny.Create<RuleConditionEditorSSID>();

		auto& editor = editorAny.Get<RuleConditionEditorSSID>();
		editor.edit_.SetText(cond.SSID);
		editor.opt_.Set((int)cond.Negative);
		editor.WhenAction = [&] { Save(); };
		editor.WhenDelete = [&, i] { DeleteCondition(i); };

		Add(editor.HSizePosZ().TopPos(h_));
		h_ += editor.GetSize().cy;

		editors_.Add(pick(editorAny));
	}
	else if(any.Is<RuleConditionAnyNetwork>()) {
		auto cond = any.Get<RuleConditionAnyNetwork>();

		Any editorAny{};
		editorAny.Create<RuleConditionEditorAnyNetwork>();

		auto& editor = editorAny.Get<RuleConditionEditorAnyNetwork>();
		editor.sw_ = (int)cond.NetworkType;
		editor.opt_.Set((int)cond.Negative);
		editor.WhenAction = [&] { Save(); };
		editor.WhenDelete = [&, i] { DeleteCondition(i); };

		Add(editor.HSizePosZ().TopPos(h_));
		h_ += editor.GetSize().cy;

		editors_.Add(pick(editorAny));
	}

	SetRectY(0, h_);
	WhenResize();
}

void RuleConditions::AddCondition(enum RuleConditionType type)
{
	int i = editors_.GetCount();

	switch(type) {
	case RULE_SSID: {
		Any editorAny{};
		editorAny.Create<RuleConditionEditorSSID>();

		auto& editor = editorAny.Get<RuleConditionEditorSSID>();
		editor.WhenAction = [&] { Save(); };
		editor.WhenDelete = [&, i] { DeleteCondition(i); };

		Add(editor.HSizePosZ().TopPos(h_));
		h_ += editor.GetSize().cy;

		editors_.Add(pick(editorAny));
		break;
	}
	case RULE_ANYNETWORK: {
		Any editorAny{};
		editorAny.Create<RuleConditionEditorAnyNetwork>();

		auto& editor = editorAny.Get<RuleConditionEditorAnyNetwork>();
		editor.WhenAction = [&] { Save(); };
		editor.WhenDelete = [&, i] { DeleteCondition(i); };

		Add(editor.HSizePosZ().TopPos(h_));
		h_ += editor.GetSize().cy;

		editors_.Add(pick(editorAny));
		break;
	}
	}

	SetRectY(0, h_);
	WhenResize();

	Save();
}

void RuleConditions::DeleteCondition(int i)
{
	if(i >= editors_.GetCount()) {
		return;
	}

	if(editors_[i].Is<RuleConditionEditorSSID>()) {
		const auto* p = &editors_[i].Get<RuleConditionEditorSSID>();
		this->RemoveChild((Ctrl*)p);
	}
	else if(editors_[i].Is<RuleConditionEditorAnyNetwork>()) {
		const auto* p = &editors_[i].Get<RuleConditionEditorAnyNetwork>();
		this->RemoveChild((Ctrl*)p);
	}
	editors_.Remove(i);

	auto h = 0;
	for(int j = 0; j < i; ++j) {
		if(editors_[j].Is<RuleConditionEditorSSID>()) {
			auto& editor = editors_[j].Get<RuleConditionEditorSSID>();
			h += editor.GetSize().cy;
		}
		else if(editors_[j].Is<RuleConditionEditorAnyNetwork>()) {
			auto& editor = editors_[j].Get<RuleConditionEditorAnyNetwork>();
			h += editor.GetSize().cy;
		}
	}
	h_ = h;

	for(int j = i; j < editors_.GetCount(); ++j) {
		if(editors_[j].Is<RuleConditionEditorSSID>()) {
			auto& editor = editors_[j].Get<RuleConditionEditorSSID>();
			editor.WhenDelete = [&, j] { DeleteCondition(j); };
			editor.HSizePosZ().TopPos(h_);
			h_ += editor.GetSize().cy;
		}
		else if(editors_[j].Is<RuleConditionEditorAnyNetwork>()) {
			auto& editor = editors_[j].Get<RuleConditionEditorAnyNetwork>();
			editor.WhenDelete = [&, j] { DeleteCondition(j); };
			editor.HSizePosZ().TopPos(h_);
			h_ += editor.GetSize().cy;
		}
	}

	SetRectY(0, h_);
	WhenResize();

	Save();
}

void RuleConditions::Check()
{
	Rule rule{};
	if(!gRuleManager->GetRule(uuid_, rule)) {
		return;
	}

	for(int i = 0; i < editors_.GetCount(); ++i) {
		auto& editorAny = editors_[i];

		if(editorAny.Is<RuleConditionEditorSSID>()) {
			auto& cond = rule.Conditions[i].Get<RuleConditionSSID>();
			auto& editor = editorAny.Get<RuleConditionEditorSSID>();
			editor.ToggleImage(cond.IsSatisfied);
		}
		else if(editorAny.Is<RuleConditionEditorAnyNetwork>()) {
			auto& cond = rule.Conditions[i].Get<RuleConditionAnyNetwork>();
			auto& editor = editorAny.Get<RuleConditionEditorAnyNetwork>();
			editor.ToggleImage(cond.IsSatisfied);
		}
	}
}

Array<Any> RuleConditions::Get() const
{
	Array<Any> array{};

	for(auto& editorAny : editors_) {
		if(editorAny.Is<RuleConditionEditorSSID>()) {
			auto& editor = editorAny.Get<RuleConditionEditorSSID>();
			auto cond = editor.Get();

			Any any{};
			any.Create<RuleConditionSSID>();
			auto& c = any.Get<RuleConditionSSID>();
			c = pick(cond);

			array.Add(pick(any));
		}
		else if(editorAny.Is<RuleConditionEditorAnyNetwork>()) {
			auto& editor = editorAny.Get<RuleConditionEditorAnyNetwork>();
			auto cond = editor.Get();

			Any any{};
			any.Create<RuleConditionAnyNetwork>();
			auto& c = any.Get<RuleConditionAnyNetwork>();
			c = pick(cond);

			array.Add(pick(any));
		}
	}

	return pick(array);
}

RuleEditor::RuleEditor()
{
	CtrlLayout(*this);

	editName_.WhenAction = [&] {
		int l{}, h{};
		editName_.GetSelection(l, h);
		WhenNameChanged(l, h);
	};
	dropTunnel_.WhenAction = [&] { Save(); };

	swType_.Add(RULE_ALL, "All", 2).Add(RULE_ANY, "Any", 2);
	swType_.WhenAction = [&] { Save(); };

	{
		addPop_.Create();
		addPop_->Add("Network name (SSID)");
		addPop_->Add("Any Ethernet/Wi-Fi");
		addPop_->WhenSelect = [&] {
			conditions_->AddCondition((RuleConditionType)addPop_->GetCursor());
			addPop_->SetCursor(0);
			btnAdd_.SetFocus();
		};
		addPop_->WhenCancel = [&] {
			addPop_->SetCursor(0);
			btnAdd_.SetFocus();
		};

		auto w = 0;
		for(int i = 0; i < addPop_->GetCount(); ++i) {
			auto x = Helper::CalcSize(addPop_->Get(i).ToString(), GetStdFont());
			w = max(w, x);
		}
		w += Zx(12);

		btnAdd_.SetImage(Upp::CtrlImg::Plus()).SetLabel("Add...");
		btnAdd_.WhenAction = [&, w] { addPop_->PopUp(&btnAdd_, w); };
	}

	scroll_.scroll.y.AutoHide(false);
	scroll_.scroll.x.Disable();

	gTunnelsManager->WhenListChanged << [&] { RefreshTunnels(); };

	gRuleManager->WhenRuleChanged << [&](Id uuid) {
		if(!conditions_.IsEmpty()) {
			conditions_->Check();
		}
	};
}

RuleEditor& RuleEditor::SetId(const Id& uuid)
{
	Rule rule{};
	if(!gRuleManager->GetRule(uuid, rule)) {
		uuid_ = Id();
		return *this;
	}

	uuid_ = uuid;

	GuiLock __;

	editName_.SetText(rule.Name);
	RefreshTunnels();
	swType_.SetData(rule.Type);

	conditions_.Clear();
	conditions_.Create();
	conditions_->WhenAction = [&] { Save(); };
	conditions_->WhenResize = [&] { RefreshConditions(); };
	for(const auto& condAny : rule.Conditions) {
		if(condAny.Is<RuleConditionSSID>()) {
			auto cond = condAny.Get<RuleConditionSSID>();

			Any any{};
			auto& c = any.Create<RuleConditionSSID>();
			c = cond;

			conditions_->AddCondition(pick(any));
		}
		else if(condAny.Is<RuleConditionAnyNetwork>()) {
			auto cond = condAny.Get<RuleConditionAnyNetwork>();

			Any any{};
			auto& c = any.Create<RuleConditionAnyNetwork>();
			c = cond;

			conditions_->AddCondition(pick(any));
		}
	}
	conditions_->SetId(rule.UUID);

	return *this;
}

Rule RuleEditor::Get() const
{
	Rule rule{};
	rule.UUID = uuid_;
	rule.Name = editName_.GetText().ToString();
	rule.TunnelId = dropTunnel_.GetIndex() >= 0 ? Id(dropTunnel_.GetKey(dropTunnel_.GetIndex())) : Id();
	rule.Type = (RuleType)((int)swType_);
	rule.Conditions = conditions_->Get();

	return pick(rule);
}

void RuleEditor::RefreshConditions()
{
	scroll_.RemoveChild(conditions_.Get());
	scroll_.Add(conditions_->HSizePosZ().TopPos(0, conditions_->GetHeight()));
	scroll_.AddPane(*conditions_);
}

void RuleEditor::RefreshTunnels()
{
	GuiLock __;

	dropTunnel_.Clear();
	auto tunnels = gTunnelsManager->GetTunnels();
	for(const auto& item : ~(tunnels)) {
		auto id = item.key;
		auto& tunnel = item.value;
		dropTunnel_.Add(id.ToString(), tunnel.Interface.Name);
	}

	Rule rule{};
	if(!gRuleManager->GetRule(uuid_, rule)) {
		return;
	}

	auto pos = tunnels.Find(rule.TunnelId);
	if(pos >= 0) {
		dropTunnel_.SetIndex(pos);
	}
}
