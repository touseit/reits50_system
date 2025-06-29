#include "DataLoader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <ctime>

void DataLoader::loadFromCSV(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("无法打开数据文件: " + filename);
    }
    
    // 跳过标题行
    std::string line;
    std::getline(file, line);
    
    // 解析数据
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        REIT reit;
        std::string field;
        
        // 按字段解析
        std::getline(iss, reit.code, ',');
        std::getline(iss, reit.name, ',');
        std::getline(iss, reit.sector, ',');
        std::getline(iss, reit.region, ',');
        
        std::getline(iss, field, ',');
        reit.market_cap = std::stod(field);
        
        std::getline(iss, field, ',');
        reit.dividend_amt = std::stod(field);
        
        std::getline(iss, field, ',');
        reit.occupancy_rate = std::stod(field);
        
        std::getline(iss, field);
        reit.debt_ratio = std::stod(field);
        
        m_data.push_back(reit);
    }
}

void DataLoader::refreshData() {
    // 模拟实时数据更新
    static time_t lastRefresh = 0;
    time_t now = time(nullptr);
    
    if (difftime(now, lastRefresh) > 300) { // 5分钟更新一次
        for (auto& reit : m_data) {
            // 模拟实时市场波动
            reit.market_cap *= (0.99 + 0.02 * (rand() / (double)RAND_MAX));
            reit.occupancy_rate = std::max(0.7, std::min(1.0, 
                reit.occupancy_rate + (0.01 * (rand() / (double)RAND_MAX - 0.5))));
        }
        lastRefresh = now;
    }
}