#pragma once

#include <nlohmann/json.hpp>
#include <string>
#include <cstddef>

namespace common {

// JSON 封装：解析与序列化全部由 nlohmann/json 库完成（header-only）
// 对外接口与旧版自定义实现保持一致（parse/dump/asXxx/operator[]等），
// 业务代码无需改动；类型不符时由库抛异常，调用方均有 try-catch 保护
class Json {
public:
    nlohmann::json raw;

    Json() = default;
    Json(std::nullptr_t) {}
    Json(bool v) : raw(v) {}
    Json(int v) : raw(v) {}
    Json(int64_t v) : raw(v) {}
    Json(double v) : raw(v) {}
    Json(const char* v) : raw(std::string(v)) {}
    Json(const std::string& v) : raw(v) {}
    Json(nlohmann::json&& j) : raw(std::move(j)) {}
    Json(const nlohmann::json& j) : raw(j) {}

    // 解析（失败抛异常，与旧实现一致）
    static Json parse(const std::string& str) {
        return Json(nlohmann::json::parse(str));
    }

    // 序列化
    std::string dump() const { return raw.dump(); }

    // 类型检查
    bool isNull() const { return raw.is_null(); }
    bool isBool() const { return raw.is_boolean(); }
    bool isInt() const { return raw.is_number_integer(); }
    bool isDouble() const { return raw.is_number(); }
    bool isString() const { return raw.is_string(); }
    bool isArray() const { return raw.is_array(); }
    bool isObject() const { return raw.is_object(); }

    // 值获取（类型不符由库抛类型错误异常）
    bool asBool() const { return raw.get<bool>(); }
    int asInt() const { return raw.get<int>(); }
    int64_t asInt64() const { return raw.get<int64_t>(); }
    double asDouble() const { return raw.get<double>(); }
    std::string asString() const { return raw.get<std::string>(); }

    // 对象操作
    bool contains(const std::string& key) const {
        return raw.is_object() && raw.contains(key);
    }

    // 写代理：支持 j["key"] = value，也支持链式读 j["key"].asString()
    class Ref {
    public:
        explicit Ref(nlohmann::json* j) : j_(j) {}

        Ref& operator=(const Ref& other) { *j_ = *other.j_; return *this; }
        Ref& operator=(const Json& v) { *j_ = v.raw; return *this; }
        Ref& operator=(const nlohmann::json& v) { *j_ = v; return *this; }

        std::string asString() const { return j_->get<std::string>(); }
        int asInt() const { return j_->get<int>(); }
        int64_t asInt64() const { return j_->get<int64_t>(); }
        bool asBool() const { return j_->get<bool>(); }
        double asDouble() const { return j_->get<double>(); }
        bool contains(const std::string& key) const {
            return j_->is_object() && j_->contains(key);
        }

        // 隐式转换：允许 const Json& x = j["key"] 场景
        operator Json() const { return Json(*j_); }

    private:
        nlohmann::json* j_;
    };

    Ref operator[](const std::string& key) { return Ref(&raw[key]); }

    // const 读：key 不存在抛异常（与旧实现一致）
    Json operator[](const std::string& key) const { return Json(raw.at(key)); }

    // 数组操作：push_back 对 null 自动转为数组（与旧实现语义一致）
    void push_back(const Json& v) { raw.push_back(v.raw); }

    // 下标访问数组，越界自动补 null（与旧实现 resize 行为一致）
    Ref operator[](size_t index) {
        if (!raw.is_array()) raw = nlohmann::json::array();
        if (raw.size() <= index) raw.resize(index + 1);
        return Ref(&raw[index]);
    }
    Json operator[](size_t index) const { return Json(raw.at(index)); }

    size_t size() const {
        if (raw.is_array() || raw.is_object()) return raw.size();
        return 0;
    }
};

} // namespace common
