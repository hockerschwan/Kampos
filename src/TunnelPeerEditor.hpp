#ifndef _TunnelPeerEditor_hpp_
#define _TunnelPeerEditor_hpp_

#include "TunnelAddressEditor.hpp"
#include "TunnelAppEditor.hpp"
#include "TunnelStructs.hpp"
#include "common.hpp"

struct TunnelEditor;

class TunnelPeerEditor : public WithTunnelPeerEditorLayout<ParentCtrl>, Moveable<TunnelPeerEditor> {
public:
	TunnelPeerEditor();

	const TunnelPeer Get() const;
	void Set(TunnelPeer& peer, int i);

	TunnelPeerEditor& SetParent(TunnelEditor* editor)
	{
		editor_ = editor;
		return *this;
	};

private:
	void Save();

	TunnelEditor* editor_;

	One<TunnelAddressEditor> allowedIPs_;
	One<TunnelAddressEditor> disallowedIPs_;
	One<TunnelAppEditor> allowedApps_;
	One<TunnelAppEditor> disallowedApps_;
};

#endif
