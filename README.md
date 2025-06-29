# 中国REITs 50 指数系统

本项目为中国REITs 50指数的自动化计算、风险监控与合规报告系统，支持作为 Windows 服务运行，并具备批量测试与报告导出功能。

## 目录结构

```
CMakeLists.txt
src/                # 源代码
  core/             # 指数计算核心
  data/             # 数据加载
  risk/             # 风险引擎
  compliance/       # 合规报告
tests/              # 测试数据与用例
config/             # 配置文件（如规则、参数）
data/               # 原始数据文件
reports/            # 生成的报告
bin/                # 可执行文件与输出
scripts/            # 辅助脚本
third_lib/          # 第三方库
docs/               # 文档
```

## 构建方法

本项目使用 CMake 构建，需先安装 CMake 3.15+ 和 Visual Studio 2019（或兼容的 MSVC 工具链）。

```sh
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

生成的可执行文件位于 `build/bin/Release/REITsIndexSystem.exe`。

## 运行方式

### 1. 普通模式

直接运行主程序，系统将自动加载数据、计算指数、监控风险并生成报告：

```sh
./REITsIndexSystem.exe
```

### 2. 测试模式

使用内置测试数据运行，并导出测试报告：

```sh
./REITsIndexSystem.exe --test
```
测试报告将生成在 `bin/test_report.csv`。

### 3. 作为 Windows 服务运行

#### 安装服务

```sh
./REITsIndexSystem.exe --install
```

#### 启动服务

服务安装后会自动启动，也可通过 Windows 服务管理器手动启动。

#### 卸载服务

```sh
./REITsIndexSystem.exe --uninstall
```

## 主要功能

- **指数计算**：自动加载 REITs 数据，按规则计算中国REITs 50指数。
- **风险监控**：实时监控成分股风险，触发警报。
- **合规报告**：自动生成合规报告并导出。
- **服务化部署**：支持以 Windows 服务方式后台运行。
- **测试支持**：一键测试并生成测试报告。

## 依赖

- C++20
- Windows 平台（服务功能依赖）
- CMake 3.15+
- Visual Studio 2019 或兼容 MSVC

## 配置文件

- 指数规则：`config/reits_index_rule.json`
- 数据文件：`data/reits_data.csv`
- 测试数据：`tests/test_data.csv`

## 相关代码入口

- 主程序入口：[src/main.cpp](src/main.cpp)
- 指数计算：[src/core/IndexCalculator.hpp](src/core/IndexCalculator.hpp)
- 数据加载：[src/data/DataLoader.hpp](src/data/DataLoader.hpp)
- 风险引擎：[src/risk/RiskEngine.hpp](src/risk/RiskEngine.hpp)
- 合规报告：[src/compliance/ComplianceReporter.hpp](src/compliance/ComplianceReporter.hpp)

---

如需详细开发文档，请参见 [docs/](docs/) 目录。