#include "RiskEngine.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>

RiskEngine::RiskEngine() {
    m_alertCallback = [](const std::string& msg) {
        std::cerr << "[风险告警] " << msg << std::endl;
    };
}

RiskEngine::~RiskEngine() {
    if (m_running) {
        stopMonitoring();
    }
}

void RiskEngine::startMonitoring() {
    if (!m_running) {
        m_running = true;
        m_monitoringThread = std::thread(&RiskEngine::monitoringThread, this);
    }
}

void RiskEngine::stopMonitoring() {
    m_running = false;
    if (m_monitoringThread.joinable()) {
        m_monitoringThread.join();
    }
}

void RiskEngine::monitoringThread() {
    using namespace std::chrono_literals;
    
    while (m_running) {
        std::this_thread::sleep_for(1s);
        
        std::vector<Component> current;
        {
            std::lock_guard lock(m_mutex);
            if (!m_currentComponents.empty()) {
                current = m_currentComponents;
            }
        }
        
        if (!current.empty()) {
            performRiskCheck(current);
        }
    }
}

void RiskEngine::setAlertCallback(AlertCallback callback) {
    std::lock_guard lock(m_mutex);
    m_alertCallback = callback;
}

void RiskEngine::performRiskCheck(const std::vector<Component>& components) {
    {
        std::lock_guard lock(m_mutex);
        m_currentComponents = components;
    }
    
    checkPositionConcentration(components);
    checkSectorConcentration(components);
    checkVolatility(components);
}

void RiskEngine::triggerCircuitBreaker() {
    if (m_alertCallback) {
        m_alertCallback("系统熔断已触发！暂停所有交易操作");
    }
}

void RiskEngine::checkPositionConcentration(const std::vector<Component>& components) {
    const double WARNING_THRESHOLD = 0.08;
    const double CRITICAL_THRESHOLD = 0.1;
    
    for (const auto& comp : components) {
        if (comp.weight >= CRITICAL_THRESHOLD && m_alertCallback) {
            m_alertCallback("REIT超限警告: " + comp.reit.name + 
                            " 权重: " + std::to_string(comp.weight * 100) + "%");
        } else if (comp.weight >= WARNING_THRESHOLD && m_alertCallback) {
            m_alertCallback("REIT接近超限: " + comp.reit.name + 
                            " 权重: " + std::to_string(comp.weight * 100) + "%");
        }
    }
}

void RiskEngine::checkSectorConcentration(const std::vector<Component>& components) {
    const std::vector<std::pair<std::string, double>> LIMITS = {
        {"物流仓储", 0.3},
        {"产业园区", 0.25},
        {"高速公路", 0.2},
        {"保障房", 0.15}
    };
    
    SectorMap sectorTotals;
    for (const auto& comp : components) {
        sectorTotals[comp.reit.sector] += comp.weight;
    }
    
    for (const auto& [sector, limit] : LIMITS) {
        auto it = sectorTotals.find(sector);
        if (it != sectorTotals.end() && it->second >= limit) {
            if (m_alertCallback) {
                m_alertCallback("行业集中度警告: " + sector + 
                                " 总权重: " + std::to_string(it->second * 100) + "%");
            }
        }
    }
}

void RiskEngine::checkVolatility(const std::vector<Component>& components) {
    // 计算平均波动率（简化版）
    double avg_volatility = 0.0;
    for (const auto& comp : components) {
        // 模拟波动率计算（实际应使用历史数据）
        avg_volatility += 0.05 * (0.8 + 0.4 * (comp.weight - 0.05));
    }
    avg_volatility /= components.size();
    
    if (avg_volatility > 0.15 && m_alertCallback) {
        m_alertCallback("波动率过高警告: " + 
                        std::to_string(avg_volatility * 100) + "%");
    }
}