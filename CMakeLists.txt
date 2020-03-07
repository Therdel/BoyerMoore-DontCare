cmake_minimum_required (VERSION 3.8)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project ("BoyerMoore")

# Add source to this project's executable.
add_executable (BoyerMoore
				"Utility.cpp"
				"MatchOrder.cpp"
				"BadCharTable.cpp"
				"BadCharTableDontCare.cpp"
				"GoodSuffixTable.cpp"
				"GoodSuffixTableDontCare.cpp"
				"BoyerMoore.cpp"
				"main.cpp")
