#ifndef _TunnelAppEditor_hpp_
#define _TunnelAppEditor_hpp_

#include "common.hpp"

struct TunnelAppEditor : public WithTunnelAppEditorLayout<ParentCtrl>, Moveable<TunnelAppEditor> {
public:
	TunnelAppEditor();

	void SetText(const String& text) { textTitle_.SetText(text); };
	SortedIndex<String> Get() const { return pick(clone(Apps_)); };

	const String ToString() const;

	void Add(const String& app);
	void Clear();

	Event<> WhenArrayAction;

private:
	SortedIndex<String> Apps_{};

	EditField edit_;
};

#endif
