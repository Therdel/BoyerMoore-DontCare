cmake_minimum_required (VERSION 3.8)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project ("BoyerMoore")

add_subdirectory(HeteroArenaAllocator)

set(BOYERMOOREDONTCARE_LIST_DIR ${CMAKE_CURRENT_LIST_DIR})

set(BOYERMOOREDONTCARE_INCLUDE_DIRS
		${HETEROARENAALLOCATOR_INCLUDE_DIRS}
		"${BOYERMOOREDONTCARE_LIST_DIR}/src"
		PARENT_SCOPE)

set(BOYERMOOREDONTCARE_SOURCES
		"${BOYERMOOREDONTCARE_LIST_DIR}/src/BoyerMooreDontCare/Utility.cpp"
		"${BOYERMOOREDONTCARE_LIST_DIR}/src/BoyerMooreDontCare/MatchOrder.cpp"
		"${BOYERMOOREDONTCARE_LIST_DIR}/src/BoyerMooreDontCare/BadCharTable.cpp"
		"${BOYERMOOREDONTCARE_LIST_DIR}/src/BoyerMooreDontCare/BadCharTableDontCare.cpp"
		"${BOYERMOOREDONTCARE_LIST_DIR}/src/BoyerMooreDontCare/GoodSuffixTable.cpp"
		"${BOYERMOOREDONTCARE_LIST_DIR}/src/BoyerMooreDontCare/GoodSuffixTableDontCare.cpp"
		"${BOYERMOOREDONTCARE_LIST_DIR}/src/BoyerMooreDontCare/BoyerMoore.cpp"
		"${BOYERMOOREDONTCARE_LIST_DIR}/src/BoyerMooreDontCare/BoyerMooreDontCare.cpp"
		PARENT_SCOPE)