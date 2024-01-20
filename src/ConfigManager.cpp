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
	if(!(create ^ b)) {
		return;
	}

	auto Run = [](const String& cmd) -> int {
		String out{};
		int ret = Sys(cmd, out);
		out.Replace("\r\n", "");
		gLogger->Log(out);
		return ret;
	};

	String cmd{};
	if(create && !b) {
		// create (stops in 3days by default)
		cmd << "schtasks /create /sc onlogon /tn \"" << GetAppName() << "\" /tr \"" << GetExeFilePath();
		cmd << "\" /ru \"" << GetUserName() << "\" /rl highest /it /f";
		if(Run(cmd) != 0) {
			gLogger->Log("Schtasks /create failed.");
			return;
		}

		// export
		String path = String() << Helper::RoamingPath() << "Task.xml";

		cmd.Clear();
		cmd << "schtasks /query /xml /tn \"" << GetAppName() << "\"";
		String xmlStr{};
		Sys(cmd, xmlStr);
		if(xmlStr.IsVoid()) {
			gLogger->Log("Schtasks export failed.");
			return;
		}

		xmlStr.Replace("\r\r\n", "\r\n");

		// delete task
		cmd.Clear();
		cmd << "schtasks /delete /tn \"\\" << GetAppName() << "\" /f";
		if(Run(cmd) != 0) {
			gLogger->Log("Schtasks /detele failed.");
			return;
		}

		// edit xml
		auto xml = ParseXML(xmlStr);
		auto i1 = xml["Task"].FindTag("Settings");
		if(i1 < 0) {
			gLogger->Log("Bad format: \"Settings\" not found in exported XML");
			return;
		}

		auto i2 = xml["Task"]["Settings"].FindTag("ExecutionTimeLimit");
		if(i2 < 0) {
			xml.At(0).At(i1).Add("ExecutionTimeLimit").AddText("PT0S");
		}
		else {
			xml.At(0).At(i1).At(i2).At(0).CreateText("PT0S");
		}

		if(!AsXMLFile(path, xml, XML_PRETTY)) {
			gLogger->Log("XML save failed.");
			return;
		}

		// import
		cmd.Clear();
		cmd << "schtasks /create /tn \"" << GetAppName() << "\" /xml \"" << path << "\"";
		if(Run(cmd) != 0) {
			gLogger->Log("Schtasks import failed.");
			return;
		}

		// delete xml
		if(!FileDelete(path)) {
			gLogger->Log("XML delete failed.");
			return;
		}
	}
	else if(!create && b) {
		cmd << "schtasks /delete /tn \"\\" << GetAppName() << "\" /f";
		Run(cmd);
	}
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
	config_.FindAdd("AutoConnect", "0");
	config_.FindAdd("AutoConnectTunnelID", "");
	config_.FindAdd("UseRules", "0");
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
