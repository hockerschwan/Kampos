#ifndef _TunnelPeerEditor_hpp_
#define _TunnelPeerEditor_hpp_

#include "TunnelAddressEditor.hpp"
#include "TunnelAppEditor.hpp"
#include "TunnelStructs.hpp"
#include "common.hpp"

class TunnelPeerEditor : public WithTunnelPeerEditorLayout<ParentCtrl>, Moveable<TunnelPeerEditor> {
public:
	TunnelPeerEditor();

	void Set(TunnelPeer& peer);

private:
	One<TunnelAddressEditor> allowedIPs_;
	One<TunnelAddressEditor> disallowedIPs_;
	One<TunnelAppEditor> allowedApps_;
	One<TunnelAppEditor> disallowedApps_;
};

#endif
