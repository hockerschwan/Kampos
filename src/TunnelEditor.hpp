#ifndef _TunnelEditor_hpp_
#define _TunnelEditor_hpp_

#include "TunnelInterfaceEditor.hpp"
#include "TunnelPeerEditor.hpp"
#include "common.hpp"

struct TunnelsPage;

struct TunnelEditor : public ParentCtrl {
public:
	TunnelEditor();

	const Id GetId() const { return pick(clone(uuid_)); };
	void SetId(const Id& uuid);

	void AddPeer();
	void DeletePeer(int i);
	void Save(bool immediate = false);

	int GetHeight() const { return ifc_->GetStdSize().cy; };

	Event<Id> WhenRefresh;

private:
	const TunnelConfig GetConfig() const;
	void Refresh()
	{
		Event<Id> h = WhenRefresh;
		h(uuid_);
	};

	Id uuid_{};

	One<TunnelInterfaceEditor> ifc_;
	Array<TunnelPeerEditor> peers_;

	Atomic timerRunning_{0};
};

#endif
