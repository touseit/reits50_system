#include "IndexCalculator.hpp"
#include <fstream>
#include <algorithm>
#include <numeric>
#include <cmath>

void IndexCalculator::loadRules(const std::string& configFile) {
    std::ifstream file(configFile);
    if (!file.is_open()) {
        throw std::runtime_error("无法打开规则配置文件: " + configFile);
    }
    
    file >> m_rules;
}

std::vector<Component> IndexCalculator::calculateComponents(
    const REITList& reits) const {
    
    std::vector<REIT> filtered = filterREITs(reits);
    std::vector<Component> components;
    double total_score = 0.0;
    
    // 计算每个REIT得分
    for (const auto& reit : filtered) {
        double score = calculateScore(reit);
        components.push_back({reit, score});
        total_score += score;
    }
    
    // 按得分排序（降序）
    std::sort(components.begin(), components.end(), 
        [](const Component& a, const Component& b) {
            return a.weight > b.weight;
        });
    
    // 取前50个REITs
    if (components.size() > 50) {
        components.resize(50);
        total_score = std::accumulate(components.begin(), components.end(), 0.0,
            [](double sum, const Component& c) { return sum + c.weight; });
    }
    
    // 计算权重
    for (auto& comp : components) {
        comp.weight = comp.weight / total_score;
    }
    
    // 应用权重限制
    applyConstraints(components);
    
    // 归一化权重
    double sum_weights = std::accumulate(components.begin(), components.end(), 0.0,
        [](double sum, const Component& c) { return sum + c.weight; });
    
    for (auto& comp : components) {
        comp.weight /= sum_weights;
    }
    
    return components;
}

double IndexCalculator::calculateIndexValue(
    const std::vector<Component>& components) const {
    
    // 使用市值加权计算指数值
    double total_value = 0.0;
    for (const auto& comp : components) {
        total_value += comp.reit.market_cap * comp.weight;
    }
    
    // 根据基准日期标准化
    double base_value = m_rules["base_value"].get<double>();
    time_t now = time(nullptr);
    return base_value * (1 + ((total_value - base_value) / base_value));
}

std::vector<REIT> IndexCalculator::filterREITs(const REITList& reits) const {
    std::vector<REIT> result;
    
    // 获取规则阈值
    double min_market_cap = m_rules["screening"]["min_market_cap"].get<double>();
    double min_dividend_yield = m_rules["screening"]["min_dividend_yield"].get<double>();
    double min_occupancy = m_rules["screening"]["min_occupancy_rate"].get<double>();
    double max_debt_ratio = m_rules["screening"]["max_debt_ratio"].get<double>();
    
    // 过滤REITs
    std::copy_if(reits.begin(), reits.end(), std::back_inserter(result),
        [=](const REIT& r) {
            return r.market_cap >= min_market_cap &&
                   (r.dividend_amt / r.market_cap) >= min_dividend_yield &&
                   r.occupancy_rate >= min_occupancy &&
                   r.debt_ratio <= max_debt_ratio;
        });
    
    return result;
}

double IndexCalculator::calculateScore(const REIT& reit) const {
    // 获取权重因子
    double div_weight = m_rules["weighting"]["dividend_weight"].get<double>();
    double market_weight = m_rules["weighting"]["market_cap_weight"].get<double>();
    
    // 计算股息得分（标准化）
    double dividend_score = (reit.dividend_amt / reit.market_cap) * div_weight;
    
    // 计算市值得分（标准化）
    double market_score = std::log(reit.market_cap + 1) * market_weight;
    
    // 应用区域因子
    double region_factor = REGION_FACTORS.count(reit.region) 
        ? REGION_FACTORS.at(reit.region) 
        : 1.0;
    
    return (dividend_score + market_score) * region_factor;
}

void IndexCalculator::applyConstraints(std::vector<Component>& components) const {
    // 1. 单REIT权重上限
    double max_single = m_rules["constraints"]["single_position_max"].get<double>();
    for (auto& comp : components) {
        if (comp.weight > max_single) {
            comp.weight = max_single;
        }
    }
    
    // 2. 行业权重上限
    json sector_constraints = m_rules["constraints"]["sector_limits"];
    SectorMap sector_totals;
    
    for (const auto& comp : components) {
        sector_totals[comp.reit.sector] += comp.weight;
    }
    
    for (const auto& constraint : sector_constraints.items()) {
        const std::string sector = constraint.key();
        double max_weight = constraint.value().get<double>();
        
        if (sector_totals.find(sector) != sector_totals.end() && 
            sector_totals[sector] > max_weight) {
            
            double adjustment = max_weight / sector_totals[sector];
            for (auto& comp : components) {
                if (comp.reit.sector == sector) {
                    comp.weight *= adjustment;
                }
            }
        }
    }
}