#ifndef _TunnelAppEditor_hpp_
#define _TunnelAppEditor_hpp_

#include "common.hpp"

struct TunnelAppEditor : public WithTunnelAppEditorLayout<ParentCtrl>, Moveable<TunnelAppEditor> {
private:
	SortedIndex<String> Apps_;

public:
	TunnelAppEditor()
	{
		CtrlLayout(*this);
		array_.AddColumn("Apps");
	};

	void SetText(const String& text) { textTitle_.SetText(text); };

	void Add(const String& app) { array_.Add(app); };

	void Clear()
	{
		Apps_.Clear();
		array_.Clear();
	};

	void Remove(const String& app)
	{
		auto i = array_.Find(app);
		if(i >= 0) {
			array_.Remove(i);
		}
	};
};

#endif
