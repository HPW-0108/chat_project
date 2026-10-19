#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <mysql/mysql.h>

namespace common {

using Row = std::unordered_map<std::string, std::string>;
using ResultSet = std::vector<Row>;

class MySQLConn {
public:
    MySQLConn();
    ~MySQLConn();

    // 连接数据库
    bool connect(const std::string& host, uint16_t port,
                 const std::string& user, const std::string& password,
                 const std::string& database);

    // 断开连接
    void close();

    // 执行查询语句
    bool executeQuery(const std::string& sql, ResultSet& result);

    // 执行非查询语句(INSERT/UPDATE/DELETE)
    bool executeUpdate(const std::string& sql, uint64_t& affectedRows);

    // 获取自增ID
    uint64_t getLastInsertId();

    // 转义字符串
    std::string escapeString(const std::string& str);

    // 检查连接是否有效
    bool ping();

    // 获取错误信息
    std::string getError() const { return lastError_; }

private:
    MYSQL*  mysql_;
    bool    connected_;
    std::string lastError_;
};

// 数据库连接池(简单版本)
class MySQLPool {
public:
    static MySQLPool& instance();

    // 初始化连接池
    bool init(const std::string& host, uint16_t port,
              const std::string& user, const std::string& password,
              const std::string& database, int poolSize = 5);

    // 获取连接
    MySQLConn* getConnection();

    // 归还连接
    void releaseConnection(MySQLConn* conn);

    // 销毁连接池
    void destroy();

private:
    MySQLPool() = default;
    ~MySQLPool() = default;

    std::vector<MySQLConn*> pool_;
    std::vector<bool>       busy_;
    std::mutex              mutex_;
    std::condition_variable cond_;

    std::string host_;
    uint16_t    port_;
    std::string user_;
    std::string password_;
    std::string database_;
    int         poolSize_;
};

} // namespace common
