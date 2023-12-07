#ifndef _TunnelAddressEditor_hpp_
#define _TunnelAddressEditor_hpp_

#include "common.hpp"

struct TunnelAddressEditor : public WithTunnelAddressEditorLayout<ParentCtrl>, Moveable<TunnelAddressEditor> {
public:
	TunnelAddressEditor();

	void SetText(const String& text) { textTitle_.SetText(text); };
	SortedIndex<String> Get() const { return pick(clone(Addresses_)); };

	const String ToString() const;

	void Add(const String& address);
	void Clear();

	Event<> WhenArrayAction;

private:
	void ArrayAction()
	{
		Event<> h = WhenArrayAction;
		h();
	};

	SortedIndex<String> Addresses_{};

	EditField edit_;
};

#endif
