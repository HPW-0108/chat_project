#pragma once

#include <string>
#include <vector>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>
#include <hiredis/hiredis.h>

namespace common {

// Redis客户端封装 - 支持同步命令和阻塞消费
class RedisClient {
public:
    RedisClient();
    ~RedisClient();

    // 连接Redis
    bool connect(const std::string& host, uint16_t port,
                 const std::string& password = "");

    // 断开连接
    void disconnect();

    // 检查是否连接
    bool isConnected() const { return ctx_ != nullptr; }

    // === 同步命令 ===

    // LPUSH: 左推入
    bool lpush(const std::string& key, const std::string& value);

    // RPOP: 右弹出(非阻塞)
    bool rpop(const std::string& key, std::string& value);

    // BLPOP: 阻塞左弹出, timeout秒
    bool blpop(const std::string& key, int timeout, std::string& value);

    // SET / GET
    bool set(const std::string& key, const std::string& value, int ttl = 0);
    bool get(const std::string& key, std::string& value);
    bool del(const std::string& key);

    // EXISTS
    bool exists(const std::string& key);

    // INCR
    bool incr(const std::string& key, int64_t& result);

    // PUBLISH / SUBSCRIBE (可选)
    bool publish(const std::string& channel, const std::string& message);

    // 执行原始命令
    redisReply* execute(const std::string& cmd);

    // 获取错误
    std::string getError() const { return lastError_; }

private:
    redisContext* ctx_;
    std::mutex mutex_;
    std::string lastError_;

    // 检查回复
    bool checkReply(redisReply* reply);
};

// Redis连接池
class RedisPool {
public:
    static RedisPool& instance();

    bool init(const std::string& host, uint16_t port,
              const std::string& password, int poolSize = 5);

    RedisClient* getConnection();
    void releaseConnection(RedisClient* conn);

    void destroy();

    // 获取连接参数(给消费者线程创建专用连接用)
    const std::string& getHost() const { return host_; }
    uint16_t getPort() const { return port_; }
    const std::string& getPassword() const { return password_; }

private:
    RedisPool() = default;
    ~RedisPool() = default;

    std::vector<RedisClient*> pool_;
    std::vector<bool> busy_;
    std::mutex mutex_;
    std::condition_variable cond_;

    std::string host_;
    uint16_t port_ = 0;
    std::string password_;
    int poolSize_ = 0;
};

// Redis队列常量
struct RedisQueue {
    // 请求队列 key格式: queue:req:{service}
    static std::string reqKey(const std::string& service) {
        return "queue:req:" + service;
    }
    // 响应队列 key格式: queue:res:{gatewayId}
    static std::string resKey(const std::string& gatewayId) {
        return "queue:res:" + gatewayId;
    }
    // 推送队列 key格式: queue:push:{userId}
    static std::string pushKey(const std::string& userId) {
        return "queue:push:" + userId;
    }
};

} // namespace common
