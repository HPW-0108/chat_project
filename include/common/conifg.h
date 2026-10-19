#pragma once

#include <string>
#include <unordered_map>

namespace common {

class Config {
public:
    static Config& instance();

    // 从文件加载配置
    bool loadFromFile(const std::string& filepath);

    // 获取配置值
    std::string getString(const std::string& key, const std::string& defaultValue = "");
    int getInt(const std::string& key, int defaultValue = 0);
    uint16_t getPort(const std::string& key, uint16_t defaultValue = 0);
    bool getBool(const std::string& key, bool defaultValue = false);

private:
    Config() = default;
    ~Config() = default;
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;

    std::unordered_map<std::string, std::string> configMap_;

    // 解析一行配置
    void parseLine(const std::string& line);
    // 去除字符串首尾空白
    static std::string trim(const std::string& s);
};

} // namespace common
