cmake_minimum_required (VERSION 3.8)

project ("BoyerMooreDontCare")

add_subdirectory(HeteroArenaAllocator)

set(LIST_DIR ${CMAKE_CURRENT_LIST_DIR})
set(INTERNAL_INCLUDE_DIRS
	${HETEROARENAALLOCATOR_INCLUDE_DIRS}
	${LIST_DIR}/src
)
set(INTERNAL_SOURCES
    ${LIST_DIR}/src/BoyerMooreDontCare/Utility.cpp
    ${LIST_DIR}/src/BoyerMooreDontCare/MatchOrder.cpp
    ${LIST_DIR}/src/BoyerMooreDontCare/BadCharTable.cpp
    ${LIST_DIR}/src/BoyerMooreDontCare/BadCharTableDontCare.cpp
    ${LIST_DIR}/src/BoyerMooreDontCare/GoodSuffixTable.cpp
    ${LIST_DIR}/src/BoyerMooreDontCare/GoodSuffixTableDontCare.cpp
    ${LIST_DIR}/src/BoyerMooreDontCare/BoyerMoore.cpp
    ${LIST_DIR}/src/BoyerMooreDontCare/BoyerMooreDontCare.cpp
)

# export config to parent CMakeLists
set(BOYERMOOREDONTCARE_INCLUDE_DIRS
    ${INTERNAL_INCLUDE_DIRS}
	PARENT_SCOPE
)
set(BOYERMOOREDONTCARE_SOURCES
    ${INTERNAL_SOURCES}
    PARENT_SCOPE
)

# examples target (optional)
add_subdirectory(examples EXCLUDE_FROM_ALL)