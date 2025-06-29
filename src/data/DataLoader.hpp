#pragma once
#include <string>
#include <vector>
#include <unordered_map>

struct REIT {
    std::string code;        // REIT代码
    std::string name;        // 名称
    std::string sector;       // 资产类型（物流/产业园等）
    std::string region;       // 区域（长三角/珠三角等）
    
    double market_cap;       // 市值（元）
    double dividend_amt;     // 年度分红金额（元）
    double occupancy_rate;   // 出租率（0.0-1.0）
    double debt_ratio;       // 负债率（0.0-1.0）
};

using REITList = std::vector<REIT>;
using SectorMap = std::unordered_map<std::string, double>;

class DataLoader {
public:
    // 从CSV文件加载REIT数据
    void loadFromCSV(const std::string& filename);
    
    // 获取当前数据
    const REITList& getCurrentData() const { return m_data; }
    
    // 定期更新数据
    void refreshData();

private:
    REITList m_data;
};