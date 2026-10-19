#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace common {

// ========== 协议常量 ==========
constexpr uint16_t MAGIC_NUMBER = 0x5A5A;      // 魔数
constexpr uint8_t  PROTOCOL_VERSION = 0x01;    // 协议版本
constexpr uint32_t HEADER_LENGTH = 12;         // 消息头长度

// ========== 服务类型 ==========
enum class ServiceType : uint8_t {
    UNKNOWN     = 0x00,
    AUTH        = 0x01,   // 登录注册服务
    CHAT        = 0x02,   // 聊天服务
    GATEWAY     = 0x03,   // 网关服务(内部)
    GUARDIAN    = 0x04,   // 守护进程(内部)
};

// ========== 消息类型 ==========
enum class MessageType : uint16_t {
    // 通用
    HEARTBEAT           = 0x0000,   // 心跳
    HEARTBEAT_ACK       = 0x0001,   // 心跳响应
    SERVICE_REGISTER    = 0x0002,   // 服务注册
    SERVICE_REGISTER_ACK= 0x0003,   // 服务注册响应
    SERVICE_DISCOVERY   = 0x0004,   // 服务发现
    SERVICE_DISCOVERY_ACK= 0x0005,  // 服务发现响应

    // 认证服务 (0x01xx)
    AUTH_REGISTER_REQ   = 0x0101,   // 注册请求
    AUTH_REGISTER_RES   = 0x0102,   // 注册响应
    AUTH_LOGIN_REQ      = 0x0103,   // 登录请求
    AUTH_LOGIN_RES      = 0x0104,   // 登录响应
    AUTH_LOGOUT_REQ     = 0x0105,   // 登出请求
    AUTH_LOGOUT_RES     = 0x0106,   // 登出响应

    // 聊天服务 (0x02xx)
    CHAT_SEND_MSG_REQ   = 0x0201,   // 发送消息请求
    CHAT_SEND_MSG_RES   = 0x0202,   // 发送消息响应
    CHAT_PUSH_MSG       = 0x0203,   // 推送消息
    CHAT_GET_HISTORY_REQ= 0x0204,   // 获取历史消息请求
    CHAT_GET_HISTORY_RES= 0x0205,   // 获取历史消息响应
    CHAT_GET_ONLINE_REQ = 0x0206,   // 获取在线用户请求
    CHAT_GET_ONLINE_RES = 0x0207,   // 获取在线用户响应
};

// ========== 响应状态码 ==========
enum class StatusCode : uint32_t {
    SUCCESS             = 0,
    FAILURE             = 1,
    USER_NOT_FOUND      = 1001,
    USER_ALREADY_EXIST  = 1002,
    PASSWORD_ERROR      = 1003,
    TOKEN_INVALID       = 1004,
    SERVICE_UNAVAILABLE = 2001,
    SERVICE_NOT_FOUND   = 2002,
    PARAM_ERROR         = 3001,
    SERVER_ERROR        = 5000,
};

// ========== 消息头 ==========
#pragma pack(push, 1)
struct MessageHeader {
    uint16_t    magic;          // 魔数 0x5A5A
    uint8_t     version;        // 协议版本
    ServiceType serviceType;    // 服务类型
    MessageType msgType;        // 消息类型
    uint32_t    bodyLength;     // 消息体长度
    uint16_t    sequence;       // 序列号

    MessageHeader()
        : magic(MAGIC_NUMBER)
        , version(PROTOCOL_VERSION)
        , serviceType(ServiceType::UNKNOWN)
        , msgType(MessageType::HEARTBEAT)
        , bodyLength(0)
        , sequence(0) {}
};
#pragma pack(pop)

// ========== 消息包 ==========
struct Message {
    MessageHeader header;
    std::string   body;  // JSON格式的消息体

    Message() = default;
    Message(ServiceType st, MessageType mt, const std::string& b = "")
        : header()
        , body(b) {
        header.serviceType = st;
        header.msgType = mt;
        header.bodyLength = static_cast<uint32_t>(body.size());
    }
};

// ========== 服务注册信息 ==========
struct ServiceInfo {
    ServiceType type;
    std::string instanceId;  // 服务实例ID
    std::string host;        // 服务地址
    uint16_t    port;        // 服务端口
    int64_t     lastHeartbeat; // 最后心跳时间
    bool        alive;       // 是否存活

    ServiceInfo()
        : type(ServiceType::UNKNOWN)
        , port(0)
        , lastHeartbeat(0)
        , alive(false) {}
};

// ========== 工具函数 ==========

// 从缓冲区解析消息头
bool parseHeader(const char* data, size_t len, MessageHeader& header);

// 序列化消息到缓冲区
std::vector<char> serializeMessage(const Message& msg);

// 生成唯一ID
std::string generateUUID();

// 获取当前时间戳(毫秒)
int64_t getCurrentTimestamp();

// 根据消息类型判断服务类型
ServiceType getServiceTypeByMessage(MessageType msgType);

} // namespace common
