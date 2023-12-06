#ifndef _TunnelInterfaceEditor_hpp_
#define _TunnelInterfaceEditor_hpp_

#include "TunnelAddressEditor.hpp"
#include "TunnelStructs.hpp"
#include "common.hpp"

struct TunnelEditor;

struct TunnelInterfaceEditor : public WithTunnelInterfaceEditorLayout<ParentCtrl> {
public:
	TunnelInterfaceEditor();

	const TunnelInterface Get() const;
	void Set(TunnelInterface& ifc);

	void SetParent(TunnelEditor* editor) { editor_ = editor; };

private:
	void Save();

	TunnelEditor* editor_;

	One<TunnelAddressEditor> addresses_;
	One<TunnelAddressEditor> dns_;
};

#endif
