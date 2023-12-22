#ifndef _TunnelCommandEditor_hpp_
#define _TunnelCommandEditor_hpp_

#include "common.hpp"

struct TunnelCommandEditor : public WithTunnelCommandEditorLayout<ParentCtrl>, Moveable<TunnelCommandEditor> {
public:
	TunnelCommandEditor();

	void SetText(const String& text) { textTitle_.SetText(text); };
	Array<String> Get() const { return pick(clone(Commands_)); };

	void Add(const String& command);
	void Clear();

	Event<> WhenArrayAction;

private:
	void AcceptEdit();

	Array<String> Commands_{};
	EditField edit_;
};

#endif
