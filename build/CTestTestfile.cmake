# CMake generated Testfile for 
# Source directory: C:/Users/Enrique/Documents/VScode/CI/Uppg3-APItester_cpp
# Build directory: C:/Users/Enrique/Documents/VScode/CI/Uppg3-APItester_cpp/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(ApiTests "C:/Users/Enrique/Documents/VScode/CI/Uppg3-APItester_cpp/build/test_api.exe")
set_tests_properties(ApiTests PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/Enrique/Documents/VScode/CI/Uppg3-APItester_cpp/CMakeLists.txt;58;add_test;C:/Users/Enrique/Documents/VScode/CI/Uppg3-APItester_cpp/CMakeLists.txt;0;")
subdirs("_deps/googletest-build")
subdirs("_deps/spdlog-build")
subdirs("_deps/cpr-build")
