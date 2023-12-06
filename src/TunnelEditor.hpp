#ifndef _TunnelEditor_hpp_
#define _TunnelEditor_hpp_

#include "TunnelInterfaceEditor.hpp"
#include "TunnelPeerEditor.hpp"
#include "common.hpp"

struct TunnelsPage;

struct TunnelEditor : public ParentCtrl {
private:
	const TunnelConfig GetConfig() const;

	TunnelsPage* parent_;

	Id uuid_{};

	One<TunnelInterfaceEditor> ifc_;
	Array<TunnelPeerEditor> peers_;

	Atomic timerRunning_{0};

public:
	TunnelEditor();

	const Id GetId() const { return pick(clone(uuid_)); };
	void SetId(const Id& uuid);

	void AddPeer();
	void DeletePeer(int i);
	void Save(bool immediate = false);

	void SetParent(TunnelsPage* parent) { parent_ = parent; };
	int GetHeight() const { return ifc_->GetStdSize().cy; };
};

#endif
