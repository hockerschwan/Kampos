#ifndef _TunnelsPage_hpp_
#define _TunnelsPage_hpp_

#include "TunnelsManager.hpp"
#include "common.hpp"

struct TunnelsPage : WithTunnelsPageLayout<ParentCtrl> {
public:
	TunnelsPage();

private:
	void Add();
	void Add_(const TunnelConfig& config);
	void Delete(const Id& uuid);
	void Duplicate(const Id& uuid);
	void Import();
	void Rename(const Id& uuid);
	void ScanTunnels();

	void ShowMenu(Bar& bar);

	Id colId_{"UUID"};
	Id colName_{"Name"};
};

#endif
