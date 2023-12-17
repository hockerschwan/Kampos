#ifndef _RuleEditor_hpp_
#define _RuleEditor_hpp_

#include "RuleConditionEditorAnyNetwork.hpp"
#include "RuleConditionEditorSSID.hpp"
#include "RuleManager.hpp"
#include "common.hpp"

struct RuleConditions : ParentCtrl {
public:
	void AddCondition(const Any& any);
	void AddCondition(RuleConditionType type);
	void DeleteCondition(int i);

	int GetCount() const { return editors_.GetCount(); };
	int GetHeight() const { return h_; };

	Array<Any> Get() const;

	Event<> WhenResize;

private:
	void Save() { WhenAction(); };

	Array<Any> editors_{};

	int h_ = 0;
};

struct RuleEditor : WithRuleLayout<ParentCtrl> {
public:
	RuleEditor();

	RuleEditor& SetId(const Id& uuid);

	Rule Get() const;

private:
	void Save() { WhenAction(); };
	void RefreshConditions();
	void RefreshTunnels();

	Id uuid_{};
	One<RuleConditions> conditions_{};

	One<PopUpList> addPop_{};
};

#endif
