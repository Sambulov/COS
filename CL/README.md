# CL

#### Add to your project as submodule
```
git submodule add https://github.com/Sambulov/CL.git
```

#### Add to your CMakeLists.txt list of files and definitions
for example:
```
get_property(CL_DEFINITIONS GLOBAL PROPERTY CL_DEFINES)
get_property(CL_SOURCES GLOBAL PROPERTY CL_SOURCES)
get_property(CL_HEADERS GLOBAL PROPERTY CL_HEADERS)

...

add_compile_definitions(${CL_DEFINITIONS})
include_directories( ${CL_HEADERS} )

add_executable(${EXECUTABLE} ...  ${CL_SOURCES} ${LINKER_SCRIPT})
```
