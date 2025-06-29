#pragma once

#include "core/IndexCalculator.hpp"
#include <vector>
#include <atomic>
#include <functional>
#include <thread>
#include <mutex>

class RiskEngine {
public:
    // 风险回调函数类型
    using AlertCallback = std::function<void(const std::string&)>;
    
    RiskEngine();
    ~RiskEngine();
    
    // 启动风险监控
    void startMonitoring();
    
    // 停止风险监控
    void stopMonitoring();
    
    // 设置风险回调
    void setAlertCallback(AlertCallback callback);
    
    // 手动检查风险
    void performRiskCheck(const std::vector<Component>& components);
    
    // 强制熔断
    void triggerCircuitBreaker();
    
private:
    // 风险监控线程
    void monitoringThread();
    
    // 检查具体风险
    void checkPositionConcentration(const std::vector<Component>& components);
    void checkSectorConcentration(const std::vector<Component>& components);
    void checkVolatility(const std::vector<Component>& components);
    
    std::thread m_monitoringThread;
    std::atomic<bool> m_running{false};
    std::mutex m_mutex;
    AlertCallback m_alertCallback;
    std::vector<Component> m_currentComponents;
};