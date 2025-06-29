#pragma once
#include "core/IndexCalculator.hpp"
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iomanip>

using json = nlohmann::json;

class ComplianceReporter {
public:
    // 生成监管报告
    std::string generateReport(const std::vector<Component>& components);
    
    // 生成XBRL格式报告
    std::string generateXbrlReport(const std::vector<Component>& components);
    
    // 导出到CSV
    void exportToCSV(const std::vector<Component>& components, 
                     const std::string& filename);
    
    // 设置报告路径
    void setReportPath(const std::string& path) {
        m_reportPath = path;
    }

private:
    std::string m_reportPath = "./reports/";
};