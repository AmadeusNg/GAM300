# CMake generated Testfile for 
# Source directory: C:/Users/eugen/Documents/GitHub/GAM300_Eugene/Dependencies/tinyxml2-master
# Build directory: C:/Users/eugen/Documents/GitHub/GAM300_Eugene/Dependencies/tinyxml2-master/Build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
if(CTEST_CONFIGURATION_TYPE MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test(xmltest "C:/Users/eugen/Documents/GitHub/GAM300_Eugene/Dependencies/tinyxml2-master/Build/Debug/xmltest.exe")
  set_tests_properties(xmltest PROPERTIES  PASS_REGULAR_EXPRESSION ", Fail 0" WORKING_DIRECTORY "C:/Users/eugen/Documents/GitHub/GAM300_Eugene/Dependencies/tinyxml2-master" _BACKTRACE_TRIPLES "C:/Users/eugen/Documents/GitHub/GAM300_Eugene/Dependencies/tinyxml2-master/CMakeLists.txt;49;add_test;C:/Users/eugen/Documents/GitHub/GAM300_Eugene/Dependencies/tinyxml2-master/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test(xmltest "C:/Users/eugen/Documents/GitHub/GAM300_Eugene/Dependencies/tinyxml2-master/Build/Release/xmltest.exe")
  set_tests_properties(xmltest PROPERTIES  PASS_REGULAR_EXPRESSION ", Fail 0" WORKING_DIRECTORY "C:/Users/eugen/Documents/GitHub/GAM300_Eugene/Dependencies/tinyxml2-master" _BACKTRACE_TRIPLES "C:/Users/eugen/Documents/GitHub/GAM300_Eugene/Dependencies/tinyxml2-master/CMakeLists.txt;49;add_test;C:/Users/eugen/Documents/GitHub/GAM300_Eugene/Dependencies/tinyxml2-master/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  add_test(xmltest "C:/Users/eugen/Documents/GitHub/GAM300_Eugene/Dependencies/tinyxml2-master/Build/MinSizeRel/xmltest.exe")
  set_tests_properties(xmltest PROPERTIES  PASS_REGULAR_EXPRESSION ", Fail 0" WORKING_DIRECTORY "C:/Users/eugen/Documents/GitHub/GAM300_Eugene/Dependencies/tinyxml2-master" _BACKTRACE_TRIPLES "C:/Users/eugen/Documents/GitHub/GAM300_Eugene/Dependencies/tinyxml2-master/CMakeLists.txt;49;add_test;C:/Users/eugen/Documents/GitHub/GAM300_Eugene/Dependencies/tinyxml2-master/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test(xmltest "C:/Users/eugen/Documents/GitHub/GAM300_Eugene/Dependencies/tinyxml2-master/Build/RelWithDebInfo/xmltest.exe")
  set_tests_properties(xmltest PROPERTIES  PASS_REGULAR_EXPRESSION ", Fail 0" WORKING_DIRECTORY "C:/Users/eugen/Documents/GitHub/GAM300_Eugene/Dependencies/tinyxml2-master" _BACKTRACE_TRIPLES "C:/Users/eugen/Documents/GitHub/GAM300_Eugene/Dependencies/tinyxml2-master/CMakeLists.txt;49;add_test;C:/Users/eugen/Documents/GitHub/GAM300_Eugene/Dependencies/tinyxml2-master/CMakeLists.txt;0;")
else()
  add_test(xmltest NOT_AVAILABLE)
endif()
