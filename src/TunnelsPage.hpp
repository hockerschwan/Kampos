#ifndef _TunnelsPage_hpp_
#define _TunnelsPage_hpp_

#include "common.hpp"

struct TunnelsPage : WithTunnelsPageLayout<ParentCtrl> {
	typedef TunnelsPage CLASSNAME;

public:
	TunnelsPage();

private:
	void Add();
	void Delete(const Id& uuid);
	void Rename(const Id& uuid);

	void ScanTunnels();

	void ShowMenu(Bar& bar);
};

#endif
