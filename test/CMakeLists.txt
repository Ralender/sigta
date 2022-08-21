include(ExternalProject)
ExternalProject_Add(gtest
  URL https://github.com/google/googletest/archive/release-1.12.1.zip
  
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}/gtest
  INSTALL_COMMAND ""
)
ExternalProject_Get_Property(gtest source_dir binary_dir)

add_executable(sigta_test
  RelPtrTest.cpp
  RTTI.cpp
)

add_dependencies(sigta_test gtest)
include_directories(${source_dir}/include)
target_link_libraries(sigta_test libgtest.a)
target_link_libraries(sigta_test libgtest_main.a)

enable_testing()

add_test(NAME    sigta_test 
         COMMAND sigta_test)
