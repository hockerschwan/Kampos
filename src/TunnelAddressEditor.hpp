#ifndef _TunnelAddressEditor_hpp_
#define _TunnelAddressEditor_hpp_

#include "common.hpp"

struct TunnelAddressEditor : public WithTunnelAddressEditorLayout<ParentCtrl>, Moveable<TunnelAddressEditor> {
private:
	SortedIndex<String> Addresses_{};

public:
	TunnelAddressEditor()
	{
		CtrlLayout(*this);
		Clear();
		array_.AddColumn("Address");
	};

	void SetText(const String& text) { textTitle_.SetText(text); };

	SortedIndex<String> Get() const { return pick(clone(Addresses_)); };

	const String ToString() const
	{
		String str{};
		for(int i = 0; i < Addresses_.GetCount(); ++i) {
			str << Addresses_[i];
			if(i != Addresses_.GetCount() - 1) {
				str << ", ";
			}
		}
		return pick(str);
	};

	void Add(const String& address) { array_.Add(address); };

	void Clear()
	{
		Addresses_.Clear();
		array_.Clear();
	};

	void Remove(const String& address)
	{
		auto i = array_.Find(address);
		if(i >= 0) {
			array_.Remove(i);
		}
	};
};

#endif
