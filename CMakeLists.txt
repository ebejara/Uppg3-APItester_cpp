cmake_minimum_required(VERSION 3.14)
project(ApiTester)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ======================================== Dependencies ========================================

include(FetchContent)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# GoogleTest + GMock
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
)
FetchContent_MakeAvailable(googletest)
include(GoogleTest)

# spdlog
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG        v1.14.1
)
FetchContent_MakeAvailable(spdlog)

# CPR + curl (kritisk del för Windows CI)
FetchContent_Declare(
    cpr
    GIT_REPOSITORY https://github.com/libcpr/cpr.git
    GIT_TAG        1.14.1
)

# ── Fixar för Windows + GitHub Actions ──
set(CPR_FORCE_WINSSL_BACKEND ON CACHE BOOL "" FORCE)      # Använd Windows inbyggda SSL
set(CPR_CURL_NOSIGNAL         ON CACHE BOOL "" FORCE)
set(CURL_DISABLE_TESTS        ON CACHE BOOL "" FORCE)     # Skippa curl:s egna tester
set(CPR_CURL_USE_LIBPSL       OFF CACHE BOOL "" FORCE)
set(CPR_USE_SYSTEM_LIB_PSL    OFF CACHE BOOL "" FORCE)

# Förhindrar det klassiska "posix_memalign" try_run-hänget på windows-latest
if(WIN32)
    set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY" CACHE STRING "" FORCE)
endif()

FetchContent_MakeAvailable(cpr)

# nlohmann/json
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.11.3
)
FetchContent_MakeAvailable(json)

# ======================================== Main executable ========================================
add_executable(APItester src/APItester.cpp)
target_include_directories(APItester PRIVATE include)
target_link_libraries(APItester PRIVATE spdlog::spdlog cpr::cpr nlohmann_json::nlohmann_json)

# ======================================== Test executable ========================================
add_executable(test_api tests/test_integration.cpp)

target_include_directories(test_api PRIVATE include)
target_link_libraries(test_api PRIVATE 
    gtest 
    gtest_main 
    gmock 
    gmock_main 
    spdlog::spdlog 
    cpr::cpr
    nlohmann_json::nlohmann_json
)

enable_testing()
gtest_discover_tests(test_api)