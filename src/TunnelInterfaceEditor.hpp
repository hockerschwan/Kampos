#ifndef _TunnelInterfaceEditor_hpp_
#define _TunnelInterfaceEditor_hpp_

#include "TunnelAddressEditor.hpp"
#include "TunnelStructs.hpp"
#include "common.hpp"

struct TunnelInterfaceEditor : public WithTunnelInterfaceEditorLayout<ParentCtrl> {
public:
	TunnelInterfaceEditor();

	void Set(TunnelInterface& ifc);

private:
	One<TunnelAddressEditor> addresses_;
	One<TunnelAddressEditor> dns_;
};

#endif
