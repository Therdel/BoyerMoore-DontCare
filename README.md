# BoyerMoore Don't Care
- BoyerMoore pattern scan with don't-care fields 
- extended Bad Character and strong Good Suffix Rule (as of [Gusfield])
- tables are arena-allocated side by side for cache-locality

## CMake Integration
If you're including this in another CMake project, you can simply clone this repo into your project directory, 
and add the following to your CMakeLists.txt:

```
add_subdirectory(BoyerMoore-DontCare)

add_executable(<your program> ${BOYERMOOREDONTCARE_SOURCES})
target_include_directories(<your program> ${BOYERMOOREDONTCARE_INCLUDE_DIRS})

```

## Reference
[Gusfield]: Dan Gusfield, "Algorithms on Strings, Trees and Sequences", Cambridge University Press, June 2010
