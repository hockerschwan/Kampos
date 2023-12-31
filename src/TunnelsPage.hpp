#ifndef _TunnelsPage_hpp_
#define _TunnelsPage_hpp_

#include "TunnelEditor.hpp"
#include "TunnelsManager.hpp"
#include "common.hpp"

struct TunnelsPage : WithTunnelsPageLayout<ParentCtrl> {
public:
	TunnelsPage();

	void Layout() override { editor_->HSizePosZ(); };

	void SetContent(const Id& uuid);

	void Connect(const Id& uuid);
	void Disconnect();

private:
	void Add();
	void Add_(const TunnelConfig& config);
	void Delete(const Id& uuid);
	void Duplicate(const Id& uuid);
	void Import();
	void Rename(const Id& uuid);
	void ScanTunnels();

	void Select(const Id& uuid);
	void ShowMenu(Bar& bar);

	Id colId_{"UUID"};
	Id colIcon_{"Icon"};
	Id colName_{"Name"};

	One<TunnelEditor> editor_;

	std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> timeConnect_{};
};

#endif
