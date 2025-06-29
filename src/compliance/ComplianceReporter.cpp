#include "ComplianceReporter.hpp"
#include <ctime>
#include <iomanip>
#include <sstream>
#include <filesystem>

namespace fs = std::filesystem;

std::string ComplianceReporter::generateReport(
    const std::vector<Component>& components) {
    
    // 确保报告目录存在
    fs::create_directories(m_reportPath);
    
    // 创建文件名
    time_t now = time(nullptr);
    tm* ltm = localtime(&now);
    std::ostringstream oss;
    oss << m_reportPath << "REITs_Report_" 
        << 1900 + ltm->tm_year 
        << std::setfill('0') << std::setw(2) << ltm->tm_mon + 1
        << std::setw(2) << ltm->tm_mday << ".json";
    
    std::string filename = oss.str();
    
    // 创建JSON结构
    json report;
    report["report_date"] = oss.str();
    report["component_count"] = components.size();
    
    json componentsJson = json::array();
    for (const auto& comp : components) {
        json compJson;
        compJson["code"] = comp.reit.code;
        compJson["name"] = comp.reit.name;
        compJson["sector"] = comp.reit.sector;
        compJson["weight"] = comp.weight;
        compJson["market_cap"] = comp.reit.market_cap;
        compJson["dividend"] = comp.reit.dividend_amt;
        
        componentsJson.push_back(compJson);
    }
    
    report["components"] = componentsJson;
    
    // 写入文件
    std::ofstream outFile(filename);
    if (!outFile) {
        throw std::runtime_error("无法打开报告文件: " + filename);
    }
    
    outFile << std::setw(4) << report << std::endl;
    return filename;
}

std::string ComplianceReporter::generateXbrlReport(
    const std::vector<Component>& components) {
    
    // XBRL模板（简化版）
    std::ostringstream xbrl;
    xbrl << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    xbrl << "<xbrl xmlns=\"http://www.xbrl.org/2003/instance\">\n";
    xbrl << "  <header>\n";
    xbrl << "    <reportingEntity>REITs指数基金管理人</reportingEntity>\n";
    time_t now = time(nullptr);
    xbrl << "    <reportDate>" << std::put_time(std::localtime(&now), "%Y-%m-%d") << "</reportDate>\n";
    xbrl << "  </header>\n";
    xbrl << "  <components>\n";
    
    for (const auto& comp : components) {
        xbrl << "    <component>\n";
        xbrl << "      <reitCode>" << comp.reit.code << "</reitCode>\n";
        xbrl << "      <weight>" << comp.weight * 100 << "%</weight>\n";
        xbrl << "      <sector>" << comp.reit.sector << "</sector>\n";
        xbrl << "      <region>" << comp.reit.region << "</region>\n";
        xbrl << "    </component>\n";
    }
    
    xbrl << "  </components>\n";
    xbrl << "</xbrl>";
    
    return xbrl.str();
}

void ComplianceReporter::exportToCSV(const std::vector<Component>& components, 
                                    const std::string& filename) {
    
    std::ofstream csv(filename);
    if (!csv) {
        throw std::runtime_error("无法创建CSV文件: " + filename);
    }
    
    // 写入标题行
    csv << "Code,Name,Sector,Region,Weight,MarketCap,Dividend,Occupancy\n";
    
    // 写入数据
    for (const auto& comp : components) {
        csv << comp.reit.code << ","
            << comp.reit.name << ","
            << comp.reit.sector << ","
            << comp.reit.region << ","
            << comp.weight << ","
            << comp.reit.market_cap << ","
            << comp.reit.dividend_amt << ","
            << comp.reit.occupancy_rate << "\n";
    }
}