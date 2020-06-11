# BoyerMoore Don't Care
- BoyerMoore pattern scan with don't-care fields 
- extended Bad Character and strong Good Suffix Rule (as of [Gusfield])
- tables are arena-allocated side by side for cache-locality

## CMake Integration
If you're including this in another CMake project, you can simply clone this repo into your project directory, 
and add the following to your CMakeLists.txt:

```
add_subdirectory(BoyerMoore-DontCare)

target_include_directories(<target> PRIVATE ${BOYERMOOREDONTCARE_INCLUDE_DIRS})
target_sources(<target> PRIVATE ${BOYERMOOREDONTCARE_SOURCES})

```

## Reference
[Gusfield]: Dan Gusfield, "Algorithms on Strings, Trees and Sequences", Cambridge University Press, June 2010
