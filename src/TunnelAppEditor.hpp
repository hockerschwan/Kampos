#ifndef _TunnelAppEditor_hpp_
#define _TunnelAppEditor_hpp_

#include "common.hpp"

struct TunnelAppEditor : public WithTunnelAppEditorLayout<ParentCtrl>, Moveable<TunnelAppEditor> {
public:
	TunnelAppEditor();

	void SetText(const String& text) { textTitle_.SetText(text); };
	Array<String> Get() const { return pick(clone(Apps_)); };

	void Add(const String& app);
	void Clear();

	Event<> WhenArrayAction;

private:
	void AcceptEdit();

	Array<String> Apps_{};

	EditField edit_;
};

#endif
