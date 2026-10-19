#pragma once

#include <muduo/net/EventLoop.h>
#include <atomic>
#include <string>
#include <thread>

namespace common {

// 看门狗：检测进程是否假死，异常时直接退出交由 docker restart 拉起
//
// 双探针设计：
//   1. EventLoop 探针：loop 内定时器每秒推进 loopBeat_，
//      事件循环卡死（死锁/阻塞回调）则时间戳停滞
//   2. 业务线程探针（可选）：业务线程调用 feedWorker() 喂狗，
//      线程卡死则时间戳停滞（如 Redis 消费线程死在某个请求里）
//
// 注意：只看内部时间戳，不受负载高低影响——服务再忙，
// 只要还在处理事件时间戳就会推进，不会误判
class Watchdog {
public:
    // checkIntervalSec: 巡检周期；timeoutSec: 时间戳停滞多久判定死亡
    Watchdog(muduo::net::EventLoop* loop,
             const std::string& serviceName,
             int checkIntervalSec = 5,
             int timeoutSec = 15);
    ~Watchdog();

    void start();
    void stop();

    // 业务工作线程喂狗（如 Redis 消费线程，要求调用周期 < timeoutSec）
    void feedWorker();

private:
    void feedLoop();   // EventLoop 定时器回调
    void checkLoop();  // 看门狗线程主循环

    muduo::net::EventLoop* loop_;
    std::string serviceName_;
    int checkIntervalSec_;
    int timeoutSec_;

    std::atomic<int64_t> loopBeat_;       // 事件循环最近活跃时间(ms)
    std::atomic<int64_t> workerBeat_;     // 业务线程最近活跃时间(ms)
    std::atomic<bool>    workerEnabled_;  // 是否监控业务线程
    std::atomic<bool>    loopStarted_;    // 事件循环已开始喂狗（启动宽限）

    std::thread thread_;
    std::atomic<bool> running_;
};

} // namespace common
