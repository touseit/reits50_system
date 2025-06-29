#pragma once
#include <vector>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include "data/DataLoader.hpp"

using json = nlohmann::json;

struct Component {
    REIT reit;
    double weight;
};

class IndexCalculator {
public:
    // 加载指数规则
    void loadRules(const std::string& configFile);
    
    // 计算指数成分
    std::vector<Component> calculateComponents(const REITList& reits) const;
    
    // 获取指数值
    double calculateIndexValue(const std::vector<Component>& components) const;
    
private:
    // 筛选合格REITs
    std::vector<REIT> filterREITs(const REITList& reits) const;
    
    // 计算REIT得分
    double calculateScore(const REIT& reit) const;
    
    // 应用限制条件
    void applyConstraints(std::vector<Component>& components) const;
    
    // 规则配置
    json m_rules;
    
    // 区域权重映射
    const std::unordered_map<std::string, double> REGION_FACTORS = {
        {"长三角", 1.2}, {"珠三角", 1.2}, 
        {"京津冀", 1.1}, {"其他", 1.0}
    };
};