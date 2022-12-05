# CMake generated Testfile for 
# Source directory: /home/sergiy/CLionProjects/FormalLang/erli-algo
# Build directory: /home/sergiy/CLionProjects/FormalLang/erli-algo/cmake-build-debug
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(test-grammar "/home/sergiy/CLionProjects/FormalLang/erli-algo/bin/test_grammar")
set_tests_properties(test-grammar PROPERTIES  _BACKTRACE_TRIPLES "/home/sergiy/CLionProjects/FormalLang/erli-algo/CMakeLists.txt;21;add_test;/home/sergiy/CLionProjects/FormalLang/erli-algo/CMakeLists.txt;0;")
add_test(test-parser "/home/sergiy/CLionProjects/FormalLang/erli-algo/bin/test_parser")
set_tests_properties(test-parser PROPERTIES  _BACKTRACE_TRIPLES "/home/sergiy/CLionProjects/FormalLang/erli-algo/CMakeLists.txt;22;add_test;/home/sergiy/CLionProjects/FormalLang/erli-algo/CMakeLists.txt;0;")
subdirs("test/test-grammar")
subdirs("test/test-parser")
