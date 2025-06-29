#include "core/IndexCalculator.hpp"
#include "data/DataLoader.hpp"
#include "risk/RiskEngine.hpp"
#include "compliance/ComplianceReporter.hpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <windows.h>
#include <cstdlib>

// Windows服务管理函数
SERVICE_STATUS g_serviceStatus;
SERVICE_STATUS_HANDLE g_statusHandle;

// 服务控制处理器
VOID WINAPI ServiceCtrlHandler(DWORD dwCtrl);

// 服务主函数
VOID WINAPI ServiceMain(DWORD argc, LPWSTR* argv);

// 系统主逻辑
void runSystem();

// Windows服务注册
bool installService();
bool uninstallService();

int main(int argc, char* argv[]) {
    // 命令行参数处理
    bool runAsService = false;
    bool testMode = false;
    
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--service") == 0) {
            runAsService = true;
        }
        else if (strcmp(argv[i], "--test") == 0) {
            testMode = true;
        }
        else if (strcmp(argv[i], "--install") == 0) {
            return installService() ? 0 : 1;
        }
        else if (strcmp(argv[i], "--uninstall") == 0) {
            return uninstallService() ? 0 : 1;
        }
    }
    
    if (runAsService) {
        SERVICE_TABLE_ENTRYW serviceTable[] = {
            {(LPWSTR)L"REITsIndexService", (LPSERVICE_MAIN_FUNCTIONW)ServiceMain},
            {NULL, NULL}
        };
        
        if (!StartServiceCtrlDispatcherW(serviceTable)) {
            std::cerr << "服务控制分发器启动失败: " << GetLastError() << std::endl;
            return 1;
        }
    }
    else if (testMode) {
        std::cout << "运行测试模式..." << std::endl;
        // 简化的测试逻辑
        DataLoader loader;
        loader.loadFromCSV("../tests/test_data.csv");
        
        IndexCalculator calculator;
        calculator.loadRules("../config/reits_index_rule.json");
        
        auto components = calculator.calculateComponents(loader.getCurrentData());
        
        ComplianceReporter reporter;
        reporter.exportToCSV(components, "test_report.csv");
        
        std::cout << "测试完成，报告已生成" << std::endl;
    }
    else {
        runSystem();
    }
    
    return 0;
}

void runSystem() {
    try {
        std::cout << "中国REITs 50指数系统启动..." << std::endl;
        
        // 初始化组件
        DataLoader loader;
        loader.loadFromCSV("../data/reits_data.csv");
        
        IndexCalculator calculator;
        calculator.loadRules("../config/reits_index_rule.json");
        
        RiskEngine riskEngine;
        riskEngine.setAlertCallback([](const std::string& msg) {
            std::cerr << "[!] " << msg << std::endl;
        });
        riskEngine.startMonitoring();
        
        ComplianceReporter reporter;
        reporter.setReportPath("../reports/");
        
        // 主循环
        while (true) {
            // 更新数据
            loader.refreshData();
            
            // 计算指数
            auto reits = loader.getCurrentData();
            auto components = calculator.calculateComponents(reits);
            double indexValue = calculator.calculateIndexValue(components);
            
            // 风险监控
            riskEngine.performRiskCheck(components);
            
            // 生成报告
            reporter.generateReport(components);
            
            // 打印状态
            std::cout << "当前指数值: " << indexValue 
                      << ", 成分股: " << components.size() 
                      << std::endl;
            
            // 每天更新一次
            std::this_thread::sleep_for(std::chrono::minutes(1)); // 实际应为86400秒
        }
    } catch (const std::exception& e) {
        std::cerr << "系统错误: " << e.what() << std::endl;
        int len = MultiByteToWideChar(CP_UTF8, 0, e.what(), -1, NULL, 0);
        std::wstring wmsg(len, L'\0');
        MultiByteToWideChar(CP_UTF8, 0, e.what(), -1, &wmsg[0], len);
        MessageBoxW(NULL, wmsg.c_str(), L"系统严重错误", MB_ICONERROR);
    }
}

// Windows服务管理实现
VOID WINAPI ServiceCtrlHandler(DWORD dwCtrl) {
    switch (dwCtrl) {
    case SERVICE_CONTROL_STOP:
        g_serviceStatus.dwCurrentState = SERVICE_STOP_PENDING;
        SetServiceStatus(g_statusHandle, &g_serviceStatus);
        // 清理资源
        g_serviceStatus.dwCurrentState = SERVICE_STOPPED;
        break;
    default:
        break;
    }
    SetServiceStatus(g_statusHandle, &g_serviceStatus);
}

VOID WINAPI ServiceMain(DWORD argc, LPWSTR* argv) {
    g_statusHandle = RegisterServiceCtrlHandlerW(L"REITsIndexService", ServiceCtrlHandler);
    
    if (!g_statusHandle) {
        return;
    }
    
    // 初始化服务状态
    ZeroMemory(&g_serviceStatus, sizeof(g_serviceStatus));
    g_serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    g_serviceStatus.dwCurrentState = SERVICE_START_PENDING;
    g_serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    SetServiceStatus(g_statusHandle, &g_serviceStatus);
    
    // 服务正在启动
    g_serviceStatus.dwCurrentState = SERVICE_RUNNING;
    SetServiceStatus(g_statusHandle, &g_serviceStatus);
    
    // 运行业务逻辑
    runSystem();
    
    // 服务停止
    g_serviceStatus.dwCurrentState = SERVICE_STOPPED;
    SetServiceStatus(g_statusHandle, &g_serviceStatus);
}

bool installService() {
    SC_HANDLE scm = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
    if (!scm) {
        std::cerr << "无法打开服务控制管理器" << std::endl;
        return false;
    }
    
    char path[MAX_PATH];
    if (!GetModuleFileNameA(NULL, path, MAX_PATH)) {
        std::cerr << "无法获取可执行文件路径" << std::endl;
        CloseServiceHandle(scm);
        return false;
    }
    
    std::wstring wServiceName = L"REITsIndexService";
    std::wstring wDisplayName = L"中国REITs50指数系统服务";
    std::wstring wBinaryPath;
    {
        int len = MultiByteToWideChar(CP_ACP, 0, path, -1, NULL, 0);
        std::wstring wPath(len, L'\0');
        MultiByteToWideChar(CP_ACP, 0, path, -1, &wPath[0], len);
        wBinaryPath = wPath.substr(0, wPath.length() - 1) + L" --service";
    }
    SC_HANDLE service = CreateServiceW(
        scm,
        wServiceName.c_str(),
        wDisplayName.c_str(),
        SERVICE_ALL_ACCESS,
        SERVICE_WIN32_OWN_PROCESS,
        SERVICE_AUTO_START,
        SERVICE_ERROR_NORMAL,
        wBinaryPath.c_str(),
        NULL, NULL, NULL, NULL, NULL
    );
    
    if (!service) {
        std::cerr << "创建服务失败: " << GetLastError() << std::endl;
        CloseServiceHandle(scm);
        return false;
    }
    
    std::cout << "服务安装成功" << std::endl;
    
    CloseServiceHandle(service);
    CloseServiceHandle(scm);
    return true;
}

bool uninstallService() {
    SC_HANDLE scm = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
    if (!scm) {
        std::cerr << "无法打开服务控制管理器" << std::endl;
        return false;
    }
    
    SC_HANDLE service = OpenServiceW(scm, L"REITsIndexService", SERVICE_STOP | DELETE);
    if (!service) {
        std::cerr << "无法打开服务: " << GetLastError() << std::endl;
        CloseServiceHandle(scm);
        return false;
    }
    
    SERVICE_STATUS status;
    if (ControlService(service, SERVICE_CONTROL_STOP, &status)) {
        std::cout << "服务停止中..." << std::endl;
        Sleep(1000);
    }
    
    if (DeleteService(service)) {
        std::cout << "服务已成功卸载" << std::endl;
    } else {
        std::cerr << "删除服务失败: " << GetLastError() << std::endl;
        CloseServiceHandle(service);
        CloseServiceHandle(scm);
        return false;
    }
    
    CloseServiceHandle(service);
    CloseServiceHandle(scm);
    return true;
}