#ifndef _ConfigManager_hpp_
#define _ConfigManager_hpp_

#include "Helper.hpp"
#include "common.hpp"

struct ConfigManager {
public:
	ConfigManager() { LoadConfig(); };
	~ConfigManager();

	String Load(const String& key, const Value defaultValue = Upp::Null);
	void Store(const String& key, const Value& value);

	void ManageTask(bool create);
	bool TaskExists();

private:
	void LoadConfig();
	bool SaveConfig();

	VectorMap<String, String> config_;
	Mutex mutex_{};
	Atomic timerRunning_{0};
};

#endif
