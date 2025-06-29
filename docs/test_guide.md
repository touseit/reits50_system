# GTest 测试说明

本项目所有单元测试和集成测试均基于 [GoogleTest (gtest)](https://github.com/google/googletest)。

## 1. 目录结构

```
tests/
  unit/           # 各核心模块单元测试
    test_data_loader_gtest.cpp
    test_index_calculator_gtest.cpp
    test_risk_engine_gtest.cpp
    test_compliance_reporter_gtest.cpp
  integration/    # 系统集成测试
    test_system_integration_gtest.cpp
  test_data.csv   # 测试数据
```

## 2. 构建与运行

### 2.1 安装 gtest

- 推荐用 vcpkg: `vcpkg install gtest`
- 或源码集成，见 gtest 官方文档

### 2.2 CMake 集成示例

```
# 添加单元测试
file(GLOB UNIT_TESTS tests/unit/*_gtest.cpp)
file(GLOB INTEGRATION_TESTS tests/integration/*_gtest.cpp)
add_executable(run_unit_tests_gtest ${UNIT_TESTS})
target_link_libraries(run_unit_tests_gtest PRIVATE gtest gtest_main ...)
add_executable(run_integration_tests_gtest ${INTEGRATION_TESTS})
target_link_libraries(run_integration_tests_gtest PRIVATE gtest gtest_main ...)
```

### 2.3 执行测试

```sh
./run_unit_tests_gtest
./run_integration_tests_gtest
```

## 3. 覆盖范围

- DataLoader：数据加载、刷新
- IndexCalculator：规则加载、成分计算、指数值
- RiskEngine：风险检查与警报
- ComplianceReporter：报告导出
- 集成测试：全流程串联

---
如需补充特殊场景或边界测试，请在对应目录添加更多用例。
