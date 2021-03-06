macro(xmt_add_library_explicit_named LIBNAME)
  xmt_add_library_explicit(${LIBNAME} ${ARGN})
  set_target_properties(${LIBNAME} PROPERTIES OUTPUT_NAME_RELEASE "${LIBNAME}")
  set_target_properties(${LIBNAME} PROPERTIES OUTPUT_NAME_DEBUG "${LIBNAME}_debug")
  set_target_properties(${LIBNAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY lib)
  set_target_properties(${LIBNAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY lib)
  set_target_properties(${LIBNAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY lib)
endmacro()

# this will set -DNAME=VAL and add an entry to
# xmt --compiled-settings via scripts/compiledSettings.py
# -> xmt/CompiledSettings.cpp
#TODO: USAGE string arg to pass to compiledSettings.py (and structured char const* settings[][3] ?)
macro(xmt_exe_linker_flags)
  SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${ARGN}")
  message(STATUS "xmt exe-only linker flags += ${ARGN}")
endmacro()

macro(xmt_linker_flags)
  SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${ARGN}")
  SET(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${ARGN}")
  SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${ARGN}")
  message(STATUS "xmt linker flags += ${ARGN}")
endmacro()

macro(xmt_compiled_setting NAME VAL)
  message(STATUS ${NAME}=${VAL})
  list(APPEND COMPILED_SETTINGS_NAMES ${NAME})
  add_definitions(-D${NAME}=${VAL})
endmacro()

macro(xmt_set NAME VAL HELP)
  message(STATUS "${NAME}=${VAL}")
  set(${NAME} ${VAL} CACHE STRING ${HELP})
endmacro()

macro(xmt_quiet_compiled_setting NAME VAL)
  add_definitions(-D${NAME}=${VAL})
endmacro()

#use this instead of FIND_PATH - uses paths in arguments before system default
macro(xmt_find_path TO FILENAME)
  FIND_PATH(${TO} ${FILENAME} ${ARGN} NO_DEFAULT_PATH)
  FIND_PATH(${TO} ${FILENAME} ${ARGN})
endmacro(xmt_find_path)

#todo: how do i test this? - not using for now
macro(strip_suffix SUF VARNAME)
  STRING(REGEX REPLACE ${SUF}"$" "" ${SUF} ${${SUF}})
endmacro(strip_suffix)

macro(xmt_find_library TO)
  find_library(${TO} ${ARGN} NO_DEFAULT_PATH)
  find_library(${TO} ${ARGN})
endmacro(xmt_find_library)

macro(xmt_add_cflags)
  message(STATUS "(xmt_add_cflags: ${ARGN})")
  set(SDL_EXTRA_CFLAGS "${SDL_EXTRA_CFLAGS} ${ARGN}")
  add_definitions("${ARGN}")
endmacro()

macro(xmt_add_ldflags)
  message("(xmt_add_ldflags: ${ARGN})")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${ARGN}")
endmacro()

macro(xmt_add_ldcflags)
  message("(xmt_ldcflag: ${ARGN})")
  xmt_add_ldflags(${ARGN})
  xmt_add_cflags(${ARGN})
endmacro()

macro(xmt_clang_stdlib c_stdc)
  set(SDL_CLANG_STDLIB_ARG "-l${c_stdc}++ -stdlib=lib${c_stdc}++")
  xmt_add_ldcflags(${SDL_CLANG_STDLIB_ARG})
endmacro()

# i tried setting these variables in a function using set(VAR "val" PARENT_SCOPE) per stackoverflow and it didn't work.
# so we use macros for setting globals
macro(xmt_enable_cpp11)
  message(STATUS "using c++11 - adding #define CPP11 1 since gcc pre-4.7 didn't bump __cplusplus")
  set(SDL_CPP11 1)
  xmt_add_cflags("-std=c++11")
  if (CMAKE_COMPILER_IS_GNUXX)
    xmt_add_cflags("-Wno-unused-function")
  endif()
  if (APPLE)
    if (${CMAKE_COMPILER_IS_CLANGXX})
      # xmt_clang_stdlib(c)
      # http://cplusplusmusings.wordpress.com/2012/07/05/clang-and-standard-libraries-on-mac-os-x/
      # but this will only work if you install newer clang + libc++ - not an old xcode version
    endif()
  endif()
  add_definitions("-DCPP11=1")

  #TODO@JG: investigate performance / regression changes between USE_BOOST_UNORDERED_SET 0 and 1
  add_definitions("-DUSE_BOOST_UNORDERED_SET=1")
  # if we don't use =1, then we risk by regression differences due to different
  # iteration orders between c++11 and not, and between compiler versions'
  # unordered_ impls. however, boost's have slightly worse performance than
  # std::tr1 and std::
endmacro()

macro(xmt_enable_debug_info)
  if (UNIX OR ANDROID OR APPLE)
    set(SDL_EXTRA_CFLAGS "${SDL_EXTRA_CFLAGS} ${SDL_DEBUG_INFO_GCC_ARG}")
    add_definitions(${SDL_EXTRA_CFLAGS})
    message(STATUS "using debugging info: ${SDL_DEBUG_INFO_GCC_ARG}")
  endif()
endmacro()

macro(xmt_gcc_flag NEW_GCCFLAG)
  if (CMAKE_COMPILER_IS_GNUXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${NEW_GCCFLAG}")
    message(STATUS "added gcc flags: ${NEW_GCCFLAG}")
  endif()
endmacro()

macro(xmt_gcc_cpp11_flag NEW_GCCFLAG)
  if (CMAKE_COMPILER_IS_GNUXX AND SDL_CPP11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${NEW_GCCFLAG}")
    message(STATUS "added gcc c++11 flags: ${NEW_GCCFLAG}")
  endif()
endmacro()

macro(xmt_finish_cflags)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SDL_EXTRA_CFLAGS}")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${SDL_EXTRA_CFLAGS}")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${SDL_EXTRA_CFLAGS}")
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELEASE} ${SDL_DEBUG_INFO_GCC_ARG}")
  set(CMAKE_CXX_FLAGS_PROFILE "${CMAKE_CXX_FLAGS_RELEASE} -pg -static-libgcc")
  set(CMAKE_CXX_FLAGS_RELWITHOUTTBBMALLOC "${CMAKE_CXX_FLAGS_RELEASE}")

  message(CMAKE_CXX_FLAGS = ${CMAKE_CXX_FLAGS})
  message(CMAKE_CXX_FLAGS_DEBUG = ${CMAKE_CXX_FLAGS_DEBUG})
  message(CMAKE_CXX_FLAGS_RELEASE = ${CMAKE_CXX_FLAGS_RELEASE})
  message(CMAKE_CXX_FLAGS_RELWITHDEBINFO = ${CMAKE_CXX_FLAGS_RELWITHDEBINFO})
endmacro()

function(xmt_add_library PROJ_NAME SOURCE_DIR)
  aux_source_directory("${SOURCE_DIR}/src" LW_PROJECT_SOURCE_FILES)
  file(GLOB_RECURSE INCS "*.hpp")
  file(GLOB_RECURSE TEMPLATE_IMPLS "${SOURCE_DIR}/src/*.ipp")
  source_group("Template Definitions" FILES ${TEMPLATE_IMPLS})
  add_library(${PROJ_NAME} ${LW_PROJECT_SOURCE_FILES} ${INCS} ${TEMPLATE_IMPLS})
  xmt_msvc_links(${PROJ_NAME})
endfunction(xmt_add_library)


function(xmt_add_executable PROJ_NAME SOURCE_DIR)
  aux_source_directory("${SOURCE_DIR}/src" LW_PROJECT_SOURCE_FILES)
  file(GLOB_RECURSE INCS "*.hpp")
  file(GLOB_RECURSE TEMPLATE_IMPLS "${SOURCE_DIR}/src/*.ipp")
  source_group("Template Definitions" FILES ${TEMPLATE_IMPLS})
  add_executable(${PROJ_NAME} ${LW_PROJECT_SOURCE_FILES} ${INCS} ${TEMPLATE_IMPLS})
  xmt_msvc_links(${PROJ_NAME})
endfunction(xmt_add_executable)

function(xmt_add_library_explicit PROJ_NAME)
  if (WIN32)
    file(GLOB_RECURSE INCS "*.hpp")
    file(GLOB_RECURSE TEMPLATE_IMPLS "${SOURCE_DIR}/src/*.ipp")
    source_group("Template Definitions" FILES ${TEMPLATE_IMPLS})
  endif()
  add_library(${PROJ_NAME} ${ARGN} ${INCS} ${TEMPLATE_IMPLS})
  xmt_msvc_links(${PROJ_NAME})
endfunction(xmt_add_library_explicit)

function(xmt_add_executable_explicit PROJ_NAME)
  if (WIN32)
    file(GLOB_RECURSE INCS "*.hpp")
    file(GLOB_RECURSE TEMPLATE_IMPLS "${SOURCE_DIR}/src/*.ipp")
    source_group("Template Definitions" FILES ${TEMPLATE_IMPLS})
  endif()
  add_executable(${PROJ_NAME} ${ARGN} ${INCS} ${TEMPLATE_IMPLS})
  xmt_msvc_links(${PROJ_NAME})
endfunction(xmt_add_executable_explicit)

function(xmt_add_test_executable PROJ_NAME SOURCE_DIR)
  aux_source_directory("${SOURCE_DIR}/test" LW_PROJECT_TEST_FILES)
  file(GLOB_RECURSE TEST_INCS "test/*.hpp")
  add_executable(${PROJ_NAME} ${LW_PROJECT_TEST_FILES} ${TEST_INCS})
  #xmt_msvc_links(${PROJ_NAME})
endfunction(xmt_add_test_executable)

# macro so we can grab xmt_UTEST_DIR var later ? seems to work as function.
function(xmt_add_unit_test PROJECT_NAME TEST_EXE SUB_UNIT_TEST_NAME)
  add_test(${PROJECT_NAME}-${SUB_UNIT_TEST_NAME} ${TEST_EXE} --catch_system_errors --detect_fp_exceptions --detect_memory_leaks --log_level=test_suite --run_test=${SUB_UNIT_TEST_NAME} --log_format=XML --log_sink=${xmt_UTEST_DIR}/${PROJECT_NAME}-testlog.xml)
endfunction(xmt_add_unit_test)

function(xmt_add_unit_tests PROJECT_NAME TEST_EXE)
  add_test(${PROJECT_NAME} ${TEST_EXE} --catch_system_errors --detect_fp_exceptions --detect_memory_leaks --log_level=test_suite --log_format=XML --log_sink=${xmt_UTEST_DIR}/${PROJECT_NAME}-testlog.xml)
endfunction(xmt_add_unit_tests)

function(xmt_unit_tests_executable PROJ_NAME SOURCE_DIR)
  xmt_add_test_executable(${PROJ_NAME} ${SOURCE_DIR})
  xmt_add_unit_tests(${PROJ_NAME}-all ${PROJ_NAME})
endfunction(xmt_unit_tests_executable)

#TODO: smilar/redundant to xmt_project_test(s): xmt_unit_tests(${LINK_DEPENDENCIES} ${PROJECT_NAME})
macro(xmt_unit_tests)
  set(PROJECT_TEST_NAME "Test${PROJECT_NAME}")
  xmt_unit_tests_executable(${PROJECT_TEST_NAME} ${PROJECT_SOURCE_DIR})
  target_link_libraries(${PROJECT_TEST_NAME} ${ARGN})
  enable_testing()
endmacro(xmt_unit_tests)


macro(xmt_msvc_links NAME)
  if (WIN32)
    if (MSVC)
      set_target_properties(${NAME} PROPERTIES
        LINK_FLAGS_DEBUG
        ${VS_MULTITHREADED_DEBUG_DLL_IGNORE_LIBRARY_FLAGS})
      set_target_properties(${NAME} PROPERTIES
        LINK_FLAGS_RELWITHDEBINFO
        ${VS_MULTITHREADED_RELEASE_DLL_IGNORE_LIBRARY_FLAGS})
      set_target_properties(${NAME} PROPERTIES
        LINK_FLAGS_RELEASE
        ${VS_MULTITHREADED_RELEASE_DLL_IGNORE_LIBRARY_FLAGS})
      set_target_properties(${NAME} PROPERTIES
        LINK_FLAGS_MINSIZEREL
        ${VS_MULTITHREADED_RELEASE_DLL_IGNORE_LIBRARY_FLAGS})
    endif()
  endif()
endmacro(xmt_msvc_links)

macro(xmt_install_targets)

  # If the user specified a target name, use it; otherwise, assume the project-name
  set(TARGET_NAME ${ARGV0})
  if(NOT TARGET_NAME)
    set(TARGET_NAME ${PROJECT_NAME})
  endif()

  set_property(TARGET ${TARGET_NAME} PROPERTY PUBLIC_HEADER ${INCS})
  install(TARGETS ${TARGET_NAME}
    CONFIGURATIONS Release
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/xmt/release/lib
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/xmt/release/lib
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/xmt/release/bin
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_PREFIX}/xmt/include/${TARGET_NAME}/include)
  install(TARGETS ${TARGET_NAME}
    CONFIGURATIONS Debug
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/xmt/debug/lib
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/xmt/debug/lib
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/xmt/debug/bin
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_PREFIX}/xmt/include/${TARGET_NAME}/include)
endmacro(xmt_install_targets)

#needs to be macro for SET to matter:
macro(xmt_rpath PATH)
  if (UNIX)
    SET(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -Wl,-rpath -Wl,${PATH}")
    SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -Wl,-rpath -Wl,${PATH}")
    #    message(STATUS "xmt_rpath link = ${CMAKE_CXX_LINK_FLAGS}")
  endif()
endmacro(xmt_rpath)

macro(xmt_libpath LIB)
  get_filename_component(RPATHDIR ${LIB} PATH )
  message(STATUS "xmt_libpath ${LIB} = ${RPATHDIR}")
  xmt_rpath(${RPATHDIR})
endmacro(xmt_libpath)

## this is the simplest way to add unit tests (where there's a single test/TestProjectName.cpp)
macro(xmt_project_test)
  # Unit tests:
  set(PROJECT_TEST_NAME "Test${PROJECT_NAME}")
  xmt_add_test_executable(${PROJECT_TEST_NAME} ${PROJECT_SOURCE_DIR})
  target_link_libraries(${PROJECT_TEST_NAME} ${PROJECT_NAME} ${LINK_DEPENDENCIES})
  enable_testing()
  xmt_add_unit_tests(${PROJECT_NAME} ${PROJECT_TEST_NAME} ${ARGN})
endmacro(xmt_project_test)

macro(xmt_tests)
  aux_source_directory("${PROJECT_SOURCE_DIR}/test" LW_PROJECT_TEST_FILES)
  file(GLOB_RECURSE TEST_INCS "test/*.hpp")
  set(PROJECT_TEST_NAME "Test${PROJECT_NAME}")

  enable_testing()

  foreach(tsrc ${LW_PROJECT_TEST_FILES})
    GET_FILENAME_COMPONENT(texe ${tsrc} NAME)
    STRING(REGEX REPLACE ".cpp$" "" texe ${texe})
    STRING(REGEX REPLACE "^Test" "" tname ${texe})
    add_executable(${texe} ${tsrc} ${TEST_INCS} ${PROJECT_SOURCE_DIR})
    target_link_libraries(${texe} ${LINK_DEPENDENCIES} ${UTIL_LIBRARIES} ${ARGN})
    #TODO: use xmt_add_unit_test macro?
    add_test(${PROJECT_NAME}-${tname} ${texe} "--catch_system_errors --detect_fp_exceptions --detect_memory_leaks --log_level=test_suite --log_format=XML --log_sink=${xmt_UTEST_DIR}/${PROJECT_NAME}-${tname}.xml")
    # xmt_msvc_links(${PROJECT_NAME}-${tname})
  endforeach(tsrc)
  #xmt_msvc_links(${PROJECT_TEST_NAME})
  #Set Project & TestProject properties.
endmacro(xmt_tests)

## when there are multiple test/Test*.cpp
macro(xmt_project_tests)
  xmt_tests(${PROJECT_NAME} ${ARGN})
endmacro(xmt_project_tests)

function(xmt_maven_project TARGET_NAME BINARY_DIR)
  set(work_dir ${BINARY_DIR}/target)
  message(STATUS "Run mvn package: \"${MAVEN_EXECUTABLE}\"")

  add_custom_target(${TARGET_NAME} ALL COMMAND ${MAVEN_EXECUTABLE} -Dmaven.project.build.directory=${work_dir} -fae -B -q -f pom.xml -Dmaven.test.skip=true clean compile package
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

endfunction(xmt_maven_project)

function(xmt_maven_project_with_profile TARGET_NAME BINARY_DIR PROFILE_NAME)
  set(work_dir ${BINARY_DIR}/target)
  message(STATUS "Run mvn package: \"${MAVEN_EXECUTABLE}\" wth profile:\"${PROFILE_NAME}\"")

  add_custom_target(${TARGET_NAME} ALL COMMAND ${MAVEN_EXECUTABLE} -P${PROFILE_NAME} -Dmaven.project.build.directory=${work_dir} -fae -B -q -f pom.xml -Dmaven.test.skip=true clean compile package
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endfunction(xmt_maven_project_with_profile)

function(xmt_maven_test TARGET_NAME BINARY_DIR)
  set(work_dir ${BINARY_DIR}/target)
  add_test(Unit_Test_${TARGET_NAME} ${MAVEN_EXECUTABLE}
    -Dmaven.project.build.directory=${work_dir} -Dtest_Dir=${CMAKE_CURRENT_SOURCE_DIR}/src/test/unit
    -f ${CMAKE_CURRENT_SOURCE_DIR}/pom.xml test )
endfunction(xmt_maven_test)

function(xmt_maven_project_with_test TARGET_NAME BINARY_DIR)
  xmt_maven_project(${TARGET_NAME} ${BINARY_DIR})
  xmt_maven_test(${TARGET_NAME} ${BINARY_DIR})
endfunction(xmt_maven_project_with_test)


function(xmt_maven_project_with_profile_test TARGET_NAME BINARY_DIR PROFILE_NAME)
  xmt_maven_project_with_profile(${TARGET_NAME} ${BINARY_DIR} ${PROFILE_NAME})
  xmt_maven_test(${TARGET_NAME} ${BINARY_DIR})
endfunction(xmt_maven_project_with_profile_test)

macro(xmt_just_unit_tests)
  set(PROJECT_TEST_NAME "Test${PROJECT_NAME}")
  xmt_add_test_executable(${PROJECT_TEST_NAME} ${PROJECT_SOURCE_DIR})
  enable_testing()
  xmt_add_unit_tests(${PROJECT_NAME} ${PROJECT_TEST_NAME})
  target_link_libraries(${PROJECT_TEST_NAME} ${LINK_DEPENDENCIES} ${ARGN})
endmacro(xmt_just_unit_tests)
