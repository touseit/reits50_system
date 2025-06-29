# 中国REITs 50指数系统 设计与开发文档

## 1. 系统架构概述

本系统旨在实现中国REITs 50指数的自动化计算、风险监控与合规报告，支持服务化部署和批量测试。系统采用模块化设计，主要分为数据加载、指数计算、风险监控、合规报告四大核心模块。

```
+-------------------+
|   DataLoader      |<--- 数据文件/定时刷新
+-------------------+
          |
          v
+-------------------+
| IndexCalculator   |<--- 规则配置
+-------------------+
          |
          v
+-------------------+
|  RiskEngine       |<--- 风险参数
+-------------------+
          |
          v
+-------------------+
|ComplianceReporter |---> 报告导出
+-------------------+
```

## 2. 主要模块说明

### 2.1 DataLoader
- 功能：负责从CSV等数据源加载REITs原始数据，并支持定时刷新。
- 主要接口：
  - `loadFromCSV(path)`：加载数据
  - `refreshData()`：刷新数据
  - `getCurrentData()`：获取当前数据

### 2.2 IndexCalculator
- 功能：根据配置规则筛选REITs，计算得分与权重，输出指数成分。
- 主要接口：
  - `loadRules(configFile)`：加载规则
  - `calculateComponents(reits)`：计算成分股及权重
  - `calculateIndexValue(components)`：计算指数值
- 设计要点：
  - 支持多因子打分、权重归一化、单股/行业权重约束
  - 规则参数通过JSON配置

### 2.3 RiskEngine
- 功能：对成分股进行风险监控，触发风险警报。
- 主要接口：
  - `setAlertCallback(cb)`：设置警报回调
  - `startMonitoring()`：启动监控
  - `performRiskCheck(components)`：风险检查

### 2.4 ComplianceReporter
- 功能：生成合规报告，支持导出CSV等格式。
- 主要接口：
  - `setReportPath(path)`：设置报告目录
  - `generateReport(components)`：生成报告
  - `exportToCSV(components, file)`：导出CSV

## 3. 数据流与流程

1. 启动后，DataLoader 加载数据。
2. IndexCalculator 根据规则筛选、打分、归一化，输出前50成分及权重。
3. RiskEngine 对成分股进行风险检查，触发警报。
4. ComplianceReporter 生成合规报告。
5. 支持定时刷新与循环处理。

## 4. 配置说明

- 规则配置：`config/reits_index_rule.json`
  - 包含筛选阈值、权重因子、约束参数等
- 数据文件：`data/reits_data.csv`、`tests/test_data.csv`
- 报告输出目录：`reports/`

## 5. 扩展与维护建议

- 支持更多数据源（如数据库、API）
- 指数规则参数化，便于灵活调整
- 风险引擎可扩展更多风险因子
- 合规报告支持多格式导出
- 日志与异常处理完善

## 6. 关键代码文件

- 主程序入口：`src/main.cpp`
- 指数计算核心：`src/core/IndexCalculator.*`
- 数据加载：`src/data/DataLoader.*`
- 风险引擎：`src/risk/RiskEngine.*`
- 合规报告：`src/compliance/ComplianceReporter.*`

## 7. 运行与部署

- 支持命令行参数：普通模式、测试模式、服务安装/卸载
- Windows服务部署：`--install`/`--uninstall`/`--service`
- 日常运行建议使用服务模式，测试可用`--test`

## 8. 参考与致谢

- 本项目部分用到 [nlohmann/json](https://github.com/nlohmann/json) 进行JSON解析。
- 感谢所有开源社区贡献者。

---
如需详细API文档或二次开发支持，请联系项目维护者。
