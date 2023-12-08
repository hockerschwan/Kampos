#include "ConfigManager.hpp"
#include "Logger.hpp"

extern std::unique_ptr<Logger> gLogger;

ConfigManager::~ConfigManager()
{
	while(timerRunning_ > 0) {
		AtomicDec(timerRunning_);
	}
	SaveConfig();
}

String ConfigManager::Load(const String& key, const Value defaultValue)
{
	mutex_.Enter();

	auto pos = config_.Find(key);
	Value res;
	if(pos < 0) {
		res = defaultValue;
		Store(key, defaultValue);
	}
	else {
		res = config_.Get(key, defaultValue);
	}

	mutex_.Leave();
	return res;
}

void ConfigManager::Store(const String& key, const Value& value)
{
	mutex_.Enter();
	auto ptr = config_.FindPtr(key);
	if(ptr == Null) {
		config_.Add(key, value.ToString());
		gLogger->Log(String() << key << ", " << value);
	}
	else {
		*ptr = value.ToString();
	}
	mutex_.Leave();

	if(timerRunning_ > 0) {
		return;
	}

	AtomicInc(timerRunning_);
	Thread t{};
	t.Run([&]() {
		t.Sleep(200);

		if(timerRunning_ == 0) {
			return;
		}
		SaveConfig();
		AtomicDec(timerRunning_);
	});
}

void ConfigManager::ManageTask(bool create)
{
	auto b = TaskExists();
	String cmd{};
	if(create && !b) {
		cmd << "schtasks /create /sc onlogon /tn \"" << GetAppName() << "\" /tr \"" << GetExeFilePath();
		cmd << "\" /ru \"" << GetUserName() << "\" /rl highest /it /f";
	}
	else if(!create && b) {
		cmd << "schtasks /delete /tn \"\\" << GetAppName() << "\" /f";
	}
	else {
		return;
	}

	String out{};
	Sys(cmd, out);
	out.Replace("\r\n", "");
	gLogger->Log(out);
}

bool ConfigManager::TaskExists()
{
	String lines{};
	Sys("schtasks /query /tn \"\\" << GetAppName() << "\" /fo csv", lines);

	for(auto& line : Split(lines, "\r\n")) {
		line.Replace("\"", "");
		auto words = Split(line, ",");
		auto name = words[0];
		name.TrimStart("\\");
		if(name == GetAppName()) {
			return true;
		}
	}
	return false;
}

void ConfigManager::LoadConfig()
{
	config_ = LoadIniFile(ConfigFile());

	// add default values if missing
	config_.FindAdd("StartHidden", "0");
	config_.FindAdd("LogDisplayLimit", "1000");
	config_.FindAdd("WireSockLogLevel", "none");
	config_.FindAdd("WireSockPath", "C:\\Program Files\\WireSock VPN Client\\bin\\wiresock-client.exe");
}

bool ConfigManager::SaveConfig()
{
	String str{};
	for(const auto& pair : ~(config_)) {
		str << pair.key << "=" << pair.value << "\n";
	}

	return SaveFile(ConfigFile(), str);
}
