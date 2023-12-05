#ifndef _TunnelEditor_hpp_
#define _TunnelEditor_hpp_

#include "TunnelInterfaceEditor.hpp"
#include "TunnelPeerEditor.hpp"
#include "common.hpp"

struct TunnelEditor : public ParentCtrl {
private:
	Id uuid_{};

	One<TunnelInterfaceEditor> ifc_;
	Array<TunnelPeerEditor> peers_;

public:
	TunnelEditor();

	const Id GetId() const { return pick(clone(uuid_)); };
	void SetId(const Id& uuid);

	int GetHeight() const { return ifc_->GetStdSize().cy; };
};

#endif
