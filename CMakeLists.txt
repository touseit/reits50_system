cmake_minimum_required(VERSION 3.15)
project(REITs50_System)

# 设置 C++20 标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 配置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 查找依赖
# find_package(nlohmann_json 3.11.2 REQUIRED)
include_directories("./third_lib")
# 添加可执行文件
add_executable(REITsIndexSystem
    src/main.cpp
    src/core/IndexCalculator.cpp
    src/data/DataLoader.cpp
    src/risk/RiskEngine.cpp
    src/compliance/ComplianceReporter.cpp
)

# 包含目录
target_include_directories(REITsIndexSystem PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# 链接库
# target_link_libraries(REITsIndexSystem PRIVATE
#     nlohmann_json::nlohmann_json
# )

# 安装配置
install(TARGETS REITsIndexSystem
    RUNTIME DESTINATION bin
    CONFIGURATIONS Release
)

install(DIRECTORY config/ DESTINATION config
    FILES_MATCHING PATTERN "*.json" PATTERN "*.ini"
)

# Windows 特定设置
if(WIN32)
    target_compile_options(REITsIndexSystem PRIVATE
        /W4
        /WX
        /guard:cf
        /sdl
    )
    
    set_target_properties(REITsIndexSystem PROPERTIES
        WIN32_EXECUTABLE ON
        LINK_FLAGS "/DYNAMICBASE"
    )
endif()

# 添加测试
enable_testing()

file(GLOB UNIT_TESTS tests/unit/*.cpp)
file(GLOB INTEGRATION_TESTS tests/integration/*.cpp)
add_executable(run_unit_tests ${UNIT_TESTS})
target_link_libraries(run_unit_tests PRIVATE ...)
add_executable(run_integration_tests ${INTEGRATION_TESTS})
target_link_libraries(run_integration_tests PRIVATE ...)
add_test(NAME UnitTests COMMAND run_unit_tests)
add_test(NAME IntegrationTests COMMAND run_integration_tests)
