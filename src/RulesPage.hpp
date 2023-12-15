#ifndef _RulesPage_hpp_
#define _RulesPage_hpp_

#include "RuleEditor.hpp"
#include "common.hpp"

struct RulesPage : WithRulesPageLayout<ParentCtrl> {
public:
	RulesPage();

	void Layout() override { editor_->HSizePosZ(); };

	void SetContent(const Id& uuid);

private:
	void Save();
	void ScanRules();
	void Select(const Id& uuid);
	void ShowMenu(Bar& bar);

	Id colId_{"UUID"};
	Id colIcon_{"Icon"};
	Id colName_{"Name"};

	One<RuleEditor> editor_{};
};

#endif
