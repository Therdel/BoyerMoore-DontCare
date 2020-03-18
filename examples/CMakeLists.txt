cmake_minimum_required (VERSION 3.8)

project("BoyerMooreDontCare_examples")

add_executable(BoyerMooreDontCare_examples "main.cpp" ${INTERNAL_SOURCES})

target_include_directories(BoyerMooreDontCare_examples PRIVATE ${INTERNAL_INCLUDE_DIRS})
		
target_compile_features(BoyerMooreDontCare_examples PRIVATE cxx_std_17)