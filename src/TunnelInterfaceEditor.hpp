#ifndef _TunnelInterfaceEditor_hpp_
#define _TunnelInterfaceEditor_hpp_

#include "TunnelAddressEditor.hpp"
#include "TunnelCommandEditor.hpp"
#include "TunnelStructs.hpp"
#include "common.hpp"

struct TunnelEditor;

struct TunnelInterfaceEditor : public WithTunnelInterfaceEditorLayout<ParentCtrl> {
public:
	TunnelInterfaceEditor();

	const TunnelInterface Get() const;
	void Set(TunnelInterface& ifc);

private:
	void Save() { Action(); };

	One<TunnelAddressEditor> addresses_;
	One<TunnelAddressEditor> dns_;
	One<TunnelCommandEditor> preup_;
	One<TunnelCommandEditor> postup_;
	One<TunnelCommandEditor> predown_;
	One<TunnelCommandEditor> postdown_;

	Id uuid_{};
};

#endif
