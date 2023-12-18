#ifndef _RuleStructs_hpp_
#define _RuleStructs_hpp_

#include "Helper.hpp"
#include <Core/Core.h>

using namespace Upp;

enum RuleType { RULE_ALL, RULE_ANY };

enum RuleConditionType { RULE_SSID, RULE_ANYNETWORK };

enum RuleNetworkType { RULE_ETHERNET, RULE_WIFI };

struct RuleConditionBase : MoveableAndDeepCopyOption<RuleConditionBase> {
public:
	RuleConditionBase(){};
	RuleConditionBase(const RuleConditionBase& x, int n)
	{
		IsSatisfied = x.IsSatisfied;
		Negative = x.Negative;
	};

	bool IsSatisfied = false;

	RuleConditionType Type{};
	bool Negative = false;
};

struct RuleConditionSSID : public RuleConditionBase {
public:
	RuleConditionSSID();
	RuleConditionSSID(const RuleConditionSSID& x, int n);

	String ToString() const;
	String Text() const;
	void Check(const Index<String>& eth, const Index<String>& wifi);

	String SSID{};
};

struct RuleConditionAnyNetwork : public RuleConditionBase {
public:
	RuleConditionAnyNetwork();
	RuleConditionAnyNetwork(const RuleConditionAnyNetwork& x, int n);

	String ToString() const;
	String Text() const;
	void Check(const Index<String>& eth, const Index<String>& wifi);

	RuleNetworkType NetworkType{};
};

struct Rule : MoveableAndDeepCopyOption<Rule> {
public:
	Id UUID{};
	String Name{};
	Id TunnelId{};
	RuleType Type{};
	Array<Any> Conditions{};

	Rule(){};
	Rule(const Rule& x, int n);

	void Check(const Index<String>& eth, const Index<String>& wifi);
	bool IsSatisfied() const { return isSatisfied_; };

	String ToString() const;

private:
	bool isSatisfied_ = false;
};

#endif
