cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

block()
    set(fileName "util.cmake")
    if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/../common/${fileName}")
        file(REMOVE "${CMAKE_CURRENT_BINARY_DIR}/tmp/${fileName}")
        file(DOWNLOAD
            "https://raw.githubusercontent.com/exqudens/exqudens-cmake/master/src/main/cmake/${fileName}"
            "${CMAKE_CURRENT_BINARY_DIR}/tmp/${fileName}"
            EXPECTED_MD5 "45e1ffe4458069384764afcfd1e8a2c0"
            STATUS downloadStatus
        )
        if(NOT "${downloadStatus}" STREQUAL "0;\"No error\"")
            message(FATAL_ERROR "downloadStatus: '${downloadStatus}'")
        endif()
        file(COPY "${CMAKE_CURRENT_BINARY_DIR}/tmp/${fileName}" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/../common")
    endif()
endblock()

include("${CMAKE_CURRENT_BINARY_DIR}/../common/util.cmake")

string(REPLACE "." ";" PRESET_NAME_ELEMENTS "${PRESET_NAME}")
if("msvc-16-x64" IN_LIST "PRESET_NAME_ELEMENTS")
    if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/../common/msvc-16-x64-toolchain.cmake")
        set_msvc_toolchain(TOOLCHAIN_CONTENT
            PROCESSOR "x86_64"
            OS "${CMAKE_HOST_SYSTEM_NAME}"
            VERSION "16"
            HOST "x64"
            TARGET "x64"
            NO_CACHE "FALSE"
            OUTPUT_FILE "${CMAKE_CURRENT_BINARY_DIR}/../common/msvc-16-x64-toolchain.cmake"
        )
    endif()
    find_program(CONAN_COMMAND NAMES "conan.exe" PATHS ENV CONAN_PATH ENV PATH REQUIRED NO_CACHE NO_DEFAULT_PATH)
elseif("gcc" IN_LIST PRESET_NAME_ELEMENTS)
    if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/../common/gcc-toolchain.cmake")
        find_file(COMPILER_PATH NAMES "gcc" PATHS ENV GCC_PATH ENV PATH REQUIRED NO_CACHE NO_DEFAULT_PATH)
        set_gnu_toolchain(TOOLCHAIN_CONTENT
            PROCESSOR "x86_64"
            OS "${CMAKE_HOST_SYSTEM_NAME}"
            PATH "${COMPILER_PATH}"
            NO_CACHE "FALSE"
            OUTPUT_FILE "${CMAKE_CURRENT_BINARY_DIR}/../common/gcc-toolchain.cmake"
        )
    endif()
    find_program(CONAN_COMMAND NAMES "conan" PATHS ENV CONAN_PATH ENV PATH REQUIRED NO_CACHE NO_DEFAULT_PATH)
endif()

file(READ "${CMAKE_CURRENT_SOURCE_DIR}/name-version.txt" INFO)
string(REPLACE ":" ";" INFO_LIST "${INFO}")
list(GET INFO_LIST 0 RAW_NAME)
list(GET INFO_LIST 1 RAW_VERSION)
string(STRIP "${RAW_NAME}" NAME)
string(STRIP "${RAW_VERSION}" VERSION)
project("${NAME}" VERSION "${VERSION}" LANGUAGES "NONE")

message("project: '${PROJECT_NAME}-${PROJECT_VERSION}'")

include("GenerateExportHeader")
enable_testing()
include(GoogleTest)

if("$ENV{CLION_IDE}")
    cmake_path(CONVERT "$ENV{PATH}" TO_CMAKE_PATH_LIST TMP_CMAKE_ENV_PATH NORMALIZE)
    list(REMOVE_AT TMP_CMAKE_ENV_PATH 0)
    cmake_path(CONVERT "${TMP_CMAKE_ENV_PATH}" TO_NATIVE_PATH_LIST TMP_NATIVE_ENV_PATH NORMALIZE)
    set("ENV{PATH}" "${TMP_NATIVE_ENV_PATH}")
endif()

find_program(CONAN_COMMAND NAMES "conan.exe" "conan" REQUIRED)
find_program(DOXYGEN_COMMAND NAMES "doxygen.exe" "doxygen" REQUIRED)

enable_language("C")
enable_language("CXX")

set(CMAKE_OBJECT_PATH_MAX 1000)
set(CMAKE_SHARED_LIBRARY_PREFIX "")
set(CMAKE_SHARED_MODULE_PREFIX "")
set(CMAKE_STATIC_LIBRARY_PREFIX "")
set(CMAKE_IMPORT_LIBRARY_PREFIX "")
set(CMAKE_STAGING_PREFIX "")
set(CMAKE_FIND_LIBRARY_PREFIXES "" "lib")
set(CMAKE_FIND_USE_CMAKE_ENVIRONMENT_PATH FALSE)
set(CMAKE_FIND_USE_CMAKE_SYSTEM_PATH FALSE)
set(CMAKE_FIND_USE_SYSTEM_ENVIRONMENT_PATH TRUE)
set(CMAKE_FIND_USE_SYSTEM_PACKAGE_REGISTRY FALSE)
set(CMAKE_FIND_USE_PACKAGE_REGISTRY FALSE)
set(CMAKE_FIND_USE_PACKAGE_ROOT_PATH FALSE)
set(CMAKE_FIND_USE_CMAKE_PATH TRUE)
set(CMAKE_FIND_PACKAGE_PREFER_CONFIG FALSE)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS FALSE)
set(CMAKE_BUILD_RPATH "\$ORIGIN")
set(CMAKE_INSTALL_RPATH "\$ORIGIN")
set(CMAKE_SKIP_BUILD_RPATH TRUE)
set(CMAKE_BUILD_RPATH_USE_ORIGIN TRUE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)
set(CMAKE_CXX_VISIBILITY_PRESET "hidden")
set(CMAKE_VISIBILITY_INLINES_HIDDEN TRUE)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(CMAKE_INSTALL_PREFIX "${PROJECT_BINARY_DIR}/cmake-install" CACHE PATH "..." FORCE)

separate_arguments(CMAKE_CXX_FLAGS NATIVE_COMMAND "${CMAKE_CXX_FLAGS}")
if(MSVC)
    if(NOT "/EHa" IN_LIST CMAKE_CXX_FLAGS AND "/EHsc" IN_LIST CMAKE_CXX_FLAGS)
        list(REMOVE_ITEM CMAKE_CXX_FLAGS "/EHsc")
        list(APPEND CMAKE_CXX_FLAGS "/EHa")
    endif()
endif()
string(JOIN " " CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})

if("${BUILD_SHARED_LIBS}")
    set(CONAN_RELEASE_COMPILER_RUNTIME "MD")
else()
    set(CONAN_RELEASE_COMPILER_RUNTIME "MT")
    if("${MINGW}")
        set(CMAKE_CXX_STANDARD_LIBRARIES "-static-libgcc -static-libstdc++ ${CMAKE_CXX_STANDARD_LIBRARIES}")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive")
    endif()
endif()

set(CMAKE_VERBOSE_MAKEFILE TRUE CACHE BOOL "" FORCE)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE CACHE BOOL "" FORCE)

set(TARGET_CMAKE_INSTALL_DEPENDS_ON "cmake-test")
option(SKIP_CMAKE_TEST "..." FALSE)
if(${SKIP_CMAKE_TEST})
    set(TARGET_CMAKE_INSTALL_DEPENDS_ON "${PROJECT_NAME}")
endif()

set_conan_architecture(CONAN_ARCH "${CMAKE_SYSTEM_PROCESSOR}")
set_conan_compiler(CONAN_COMPILER "${CMAKE_CXX_COMPILER_ID}")
set_conan_compiler_version(CONAN_COMPILER_VERSION "${CMAKE_CXX_COMPILER_ID}" "${CMAKE_CXX_COMPILER_VERSION}" MAX_ELEMENTS "1")
set_conan_compiler_runtime(CONAN_COMPILER_RUNTIME "${CMAKE_MSVC_RUNTIME_LIBRARY}")

set_python_boolean(CONAN_INTERFACE_ONLY "${INTERFACE_ONLY}")
set_python_boolean(CONAN_BUILD_SHARED_LIBS "${BUILD_SHARED_LIBS}")

set_conan_settings(CONAN_SETTINGS
    "arch=${CONAN_ARCH}"
    "os=${CMAKE_SYSTEM_NAME}"
    "compiler=${CONAN_COMPILER}"
    "compiler.version=${CONAN_COMPILER_VERSION}"
    "compiler.runtime=${CONAN_COMPILER_RUNTIME}"
    "compiler.libcxx=${CONAN_COMPILER_LIBCXX}"
    "build_type=${CMAKE_BUILD_TYPE}"
)
set_conan_options(CONAN_OPTIONS
    "interface=${CONAN_INTERFACE_ONLY}"
    "shared=${CONAN_BUILD_SHARED_LIBS}"
)
set("CONAN_INSTALL_PREFIX" "${PROJECT_BINARY_DIR}/conan-install")
if(NOT EXISTS "${CONAN_INSTALL_PREFIX}")
    execute_process(
        COMMAND "${CONAN_COMMAND}"
                install
                "${PROJECT_SOURCE_DIR}/conanfile.py"
                --install-folder
                "${CONAN_INSTALL_PREFIX}"
                ${CONAN_SETTINGS}
                ${CONAN_OPTIONS}
        COMMAND_ECHO "STDERR"
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ENCODING "UTF-8"
        COMMAND_ERROR_IS_FATAL ANY
    )
endif()
if(NOT EXISTS "${CONAN_INSTALL_PREFIX}/conan-packages.cmake")
    message(FATAL_ERROR "Not exists: '${CONAN_INSTALL_PREFIX}/conan-packages.cmake'!")
endif()
include("${CONAN_INSTALL_PREFIX}/conan-packages.cmake")
set(CMAKE_MODULE_PATH "${CONAN_INSTALL_PREFIX}")
foreach(
    conanPackageName
    cmakePackageName
    cmakePackageVersion
    cmakePackagePath
    IN ZIP_LISTS
    "${PROJECT_NAME}_CONAN_PACKAGE_NAMES"
    "${PROJECT_NAME}_CMAKE_PACKAGE_NAMES"
    "${PROJECT_NAME}_CMAKE_PACKAGE_VERSIONS"
    "${PROJECT_NAME}_CMAKE_PACKAGE_PATHS"
)
    if(
        "" STREQUAL "${conanPackageName}"
        OR "" STREQUAL "${cmakePackageName}"
        OR "" STREQUAL "${cmakePackageVersion}"
        OR "" STREQUAL "${cmakePackagePath}"
    )
        string(JOIN " " errorMessage
            "Empty value 'conanPackageName': '${conanPackageName}'"
            "or 'cmakePackageName': '${cmakePackageName}'"
            "or 'cmakePackageVersion': '${cmakePackageVersion}'"
            "or 'cmakePackagePath': '${cmakePackagePath}'!"
        )
        message(FATAL_ERROR "${errorMessage}")
    elseif(NOT IS_DIRECTORY "${cmakePackagePath}")
        message(FATAL_ERROR "Not is directory 'cmakePackagePath': '${cmakePackagePath}'!")
    endif()

    list(APPEND CMAKE_PREFIX_PATH "${cmakePackagePath}")

    if("gtest" STREQUAL "${conanPackageName}")
        file(REMOVE "${CONAN_INSTALL_PREFIX}/Find${cmakePackageName}.cmake")
        list(APPEND NOT_FOUND_PACKAGE_NAMES "${cmakePackageName}")
        find_package("${cmakePackageName}" "${cmakePackageVersion}" EXACT QUIET CONFIG)
    else()
        file(REMOVE "${CONAN_INSTALL_PREFIX}/Find${cmakePackageName}.cmake")
        list(APPEND NOT_FOUND_PACKAGE_NAMES "${cmakePackageName}")
        find_package("${cmakePackageName}" "${cmakePackageVersion}" EXACT QUIET CONFIG)
    endif()

endforeach()
set_not_found_package_names(NOT_FOUND_PACKAGE_NAMES ${NOT_FOUND_PACKAGE_NAMES})
if(NOT "" STREQUAL "${NOT_FOUND_PACKAGE_NAMES}")
    foreach(package ${NOT_FOUND_PACKAGE_NAMES})
        message(SEND_ERROR "${package}_FOUND: '${${package}_FOUND}' ${package}_NOT_FOUND_MESSAGE: '${${package}_NOT_FOUND_MESSAGE}'")
    endforeach()
    message(FATAL_ERROR "Some package not found!")
endif()

set("${PROJECT_NAME}-header-files"
    "src/main/cpp/exqudens/Sockets.hpp"
    "src/main/cpp/exqudens/SocketServer.hpp"
    "src/main/cpp/exqudens/SocketClient.hpp"
)
set("${PROJECT_NAME}-source-files"
    "src/main/cpp/exqudens/${CMAKE_SYSTEM_NAME}Sockets.cpp"
    "src/main/cpp/exqudens/${CMAKE_SYSTEM_NAME}SocketServer.cpp"
    "src/main/cpp/exqudens/${CMAKE_SYSTEM_NAME}SocketClient.cpp"
)
if("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
    set(CMAKE_CXX_STANDARD_LIBRARIES "${CMAKE_CXX_STANDARD_LIBRARIES} Ws2_32.lib")
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    #set(CMAKE_CXX_STANDARD_LIBRARIES "${CMAKE_CXX_STANDARD_LIBRARIES} Ws2_32.lib")
endif()
if("${INTERFACE_ONLY}")
    generate_interface_only_files(GENERATED_SOURCES
        SRC_DIRECTORY "${PROJECT_SOURCE_DIR}"
        SRC_BASE_DIRECTORY "${PROJECT_SOURCE_DIR}/src/main/cpp"
        DST_BASE_DIRECTORY "${PROJECT_BINARY_DIR}/generated/src/main/cpp"
        HEADER_FILES ${${PROJECT_NAME}-header-files}
        SOURCE_FILES ${${PROJECT_NAME}-source-files}
        HEADER_SOURCE_MAPS "Sockets>${CMAKE_SYSTEM_NAME}Sockets"
                           "SocketServer>${CMAKE_SYSTEM_NAME}SocketServer"
                           "SocketClient>${CMAKE_SYSTEM_NAME}SocketClient"
    )

    add_library("${PROJECT_NAME}" INTERFACE
        ${GENERATED_SOURCES}
    )
    file(MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/generated/src/main/cpp/exqudens")
    string(CONCAT EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES_CONTENT
        "#ifndef EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES\n"
        "#  define EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES inline\n"
        "#endif /* EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES */\n"
    )
    file(WRITE "${PROJECT_BINARY_DIR}/generated/src/main/cpp/exqudens/socket_export.hpp"
        "#ifndef EXQUDENS_SOCKET_EXPORT_H\n"
        "#define EXQUDENS_SOCKET_EXPORT_H\n"
        "\n"
        "#ifndef EXQUDENS_SOCKET_EXPORT\n"
        "#  define EXQUDENS_SOCKET_EXPORT\n"
        "#endif /* EXQUDENS_SOCKET_EXPORT */\n"
        "\n"
        "${EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES_CONTENT}"
        "\n"
        "#endif /* EXQUDENS_SOCKET_EXPORT_H */\n"
    )
    target_include_directories("${PROJECT_NAME}" INTERFACE
        "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/generated/src/main/cpp>"
        "$<INSTALL_INTERFACE:include>"
    )
else()
    add_library("${PROJECT_NAME}"
        ${${PROJECT_NAME}-header-files}
        ${${PROJECT_NAME}-source-files}
    )
    string(CONCAT EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES_CONTENT
        "#ifndef EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES\n"
        "#  define EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES\n"
        "#endif /* EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES */\n"
    )
    generate_export_header("${PROJECT_NAME}"
        BASE_NAME "EXQUDENS_SOCKET"
        EXPORT_FILE_NAME "generated/src/main/cpp/exqudens/socket_export.hpp"
        CUSTOM_CONTENT_FROM_VARIABLE "EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES_CONTENT"
    )
    if(NOT "${BUILD_SHARED_LIBS}")
        set_target_properties("${PROJECT_NAME}" PROPERTIES COMPILE_FLAGS -DEXQUDENS_SOCKET_STATIC_DEFINE)
    endif()
    target_include_directories("${PROJECT_NAME}" PUBLIC
        "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/generated/src/main/cpp>"
        "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/main/cpp>"
        "$<INSTALL_INTERFACE:include>"
    )
    set_target_properties("${PROJECT_NAME}" PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY                "${PROJECT_BINARY_DIR}/main/bin"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE        "${PROJECT_BINARY_DIR}/main/bin"
        RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${PROJECT_BINARY_DIR}/main/bin"
        RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL     "${PROJECT_BINARY_DIR}/main/bin"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG          "${PROJECT_BINARY_DIR}/main/bin"

        ARCHIVE_OUTPUT_DIRECTORY                "${PROJECT_BINARY_DIR}/main/lib"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASE        "${PROJECT_BINARY_DIR}/main/lib"
        ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO "${PROJECT_BINARY_DIR}/main/lib"
        ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL     "${PROJECT_BINARY_DIR}/main/lib"
        ARCHIVE_OUTPUT_DIRECTORY_DEBUG          "${PROJECT_BINARY_DIR}/main/lib"

        LIBRARY_OUTPUT_DIRECTORY                "${PROJECT_BINARY_DIR}/main/lib"
        LIBRARY_OUTPUT_DIRECTORY_RELEASE        "${PROJECT_BINARY_DIR}/main/lib"
        LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO "${PROJECT_BINARY_DIR}/main/lib"
        LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL     "${PROJECT_BINARY_DIR}/main/lib"
        LIBRARY_OUTPUT_DIRECTORY_DEBUG          "${PROJECT_BINARY_DIR}/main/lib"
    )
endif()
set_property(TARGET "${PROJECT_NAME}" PROPERTY "VERSION" "${PROJECT_VERSION}")
set_property(TARGET "${PROJECT_NAME}" PROPERTY "SOVERSION" "${PROJECT_VERSION}")
set_property(TARGET "${PROJECT_NAME}" PROPERTY "INTERFACE_${PROJECT_NAME}_MAJOR_VERSION" "${PROJECT_VERSION}")
set_property(TARGET "${PROJECT_NAME}" APPEND PROPERTY "COMPATIBLE_INTERFACE_STRING" "${PROJECT_NAME}_MAJOR_VERSION")
install(
    EXPORT "${PROJECT_NAME}-targets"
    DESTINATION "cmake"
    NAMESPACE "${PROJECT_NAME}::"
    FILE "${PROJECT_NAME}-targets.cmake"
)
include(CMakePackageConfigHelpers)
configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/package-config.cmake.in"
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
    INSTALL_DESTINATION "cmake"
)
write_basic_package_version_file(
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
    VERSION "${PROJECT_VERSION}"
    COMPATIBILITY "ExactVersion"
)
install(
    FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
    DESTINATION "cmake"
)
if("${INTERFACE_ONLY}")
    install(
        DIRECTORY "${PROJECT_BINARY_DIR}/generated/src/main/cpp/"
        DESTINATION "include"
        FILES_MATCHING
        PATTERN "*.h"
        PATTERN "*.hpp"
    )
else()
    install(
        DIRECTORY "${PROJECT_BINARY_DIR}/generated/src/main/cpp/"
                  "src/main/cpp/"
        DESTINATION "include"
        FILES_MATCHING
        PATTERN "*.h"
        PATTERN "*.hpp"
    )
endif()
install(
    TARGETS "${PROJECT_NAME}"
    EXPORT "${PROJECT_NAME}-targets"
    INCLUDES DESTINATION "include"
    RUNTIME DESTINATION "bin"
    LIBRARY DESTINATION "lib"
    ARCHIVE DESTINATION "lib"
)

add_library("test-lib" STATIC
    "src/test/cpp/TestApplication.hpp"
    "src/test/cpp/TestApplication.cpp"
    "src/test/cpp/TestUtils.hpp"
    "src/test/cpp/TestUtils.cpp"
    "src/test/cpp/TestThreadPool.hpp"
    "src/test/cpp/TestThreadPool.cpp"
    "src/test/cpp/exqudens/Tests.hpp"
)
target_include_directories("test-lib" PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/test/cpp>"
    "$<INSTALL_INTERFACE:include>"
)
target_link_libraries("test-lib" PUBLIC
    "${PROJECT_NAME}"
    "GTest::gmock"
    "GTest::gtest"
    "GTest::gmock_main"
    "GTest::gtest_main"
)
set_target_properties("test-lib" PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY                "${PROJECT_BINARY_DIR}/test/bin"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE        "${PROJECT_BINARY_DIR}/test/bin"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${PROJECT_BINARY_DIR}/test/bin"
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL     "${PROJECT_BINARY_DIR}/test/bin"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG          "${PROJECT_BINARY_DIR}/test/bin"

    ARCHIVE_OUTPUT_DIRECTORY                "${PROJECT_BINARY_DIR}/test/lib"
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE        "${PROJECT_BINARY_DIR}/test/lib"
    ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO "${PROJECT_BINARY_DIR}/test/lib"
    ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL     "${PROJECT_BINARY_DIR}/test/lib"
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG          "${PROJECT_BINARY_DIR}/test/lib"

    LIBRARY_OUTPUT_DIRECTORY                "${PROJECT_BINARY_DIR}/test/lib"
    LIBRARY_OUTPUT_DIRECTORY_RELEASE        "${PROJECT_BINARY_DIR}/test/lib"
    LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO "${PROJECT_BINARY_DIR}/test/lib"
    LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL     "${PROJECT_BINARY_DIR}/test/lib"
    LIBRARY_OUTPUT_DIRECTORY_DEBUG          "${PROJECT_BINARY_DIR}/test/lib"
)

add_executable("test-app" "src/test/cpp/main.cpp")
target_link_libraries("test-app"
    "test-lib"
)
set_target_properties("test-app" PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY                "${PROJECT_BINARY_DIR}/test/bin"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE        "${PROJECT_BINARY_DIR}/test/bin"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${PROJECT_BINARY_DIR}/test/bin"
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL     "${PROJECT_BINARY_DIR}/test/bin"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG          "${PROJECT_BINARY_DIR}/test/bin"

    ARCHIVE_OUTPUT_DIRECTORY                "${PROJECT_BINARY_DIR}/test/lib"
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE        "${PROJECT_BINARY_DIR}/test/lib"
    ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO "${PROJECT_BINARY_DIR}/test/lib"
    ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL     "${PROJECT_BINARY_DIR}/test/lib"
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG          "${PROJECT_BINARY_DIR}/test/lib"

    LIBRARY_OUTPUT_DIRECTORY                "${PROJECT_BINARY_DIR}/test/lib"
    LIBRARY_OUTPUT_DIRECTORY_RELEASE        "${PROJECT_BINARY_DIR}/test/lib"
    LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO "${PROJECT_BINARY_DIR}/test/lib"
    LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL     "${PROJECT_BINARY_DIR}/test/lib"
    LIBRARY_OUTPUT_DIRECTORY_DEBUG          "${PROJECT_BINARY_DIR}/test/lib"
)
if("${BUILD_SHARED_LIBS}")
    if("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
        add_custom_command(TARGET "test-app"
            PRE_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_directory "$<TARGET_PROPERTY:${PROJECT_NAME},RUNTIME_OUTPUT_DIRECTORY>" "$<TARGET_PROPERTY:test-app,RUNTIME_OUTPUT_DIRECTORY>"
            COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CONAN_INSTALL_PREFIX}/bin" "$<TARGET_PROPERTY:test-app,RUNTIME_OUTPUT_DIRECTORY>"
            VERBATIM
            USES_TERMINAL
        )
    else()
        add_custom_command(TARGET "test-app"
            PRE_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_directory "$<TARGET_PROPERTY:${PROJECT_NAME},LIBRARY_OUTPUT_DIRECTORY>" "$<TARGET_PROPERTY:test-app,RUNTIME_OUTPUT_DIRECTORY>"
            COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CONAN_INSTALL_PREFIX}/lib" "$<TARGET_PROPERTY:test-app,RUNTIME_OUTPUT_DIRECTORY>"
            VERBATIM
            USES_TERMINAL
        )
    endif()
endif()

gtest_discover_tests("test-app"
    WORKING_DIRECTORY "${PROJECT_BINARY_DIR}/test/bin"
    PROPERTIES ENVIRONMENT "PARENT_PATH=;PATH="
    XML_OUTPUT_DIR "${PROJECT_BINARY_DIR}/test/report/xml"
)

add_custom_target("cmake-test"
    COMMAND "${CMAKE_CTEST_COMMAND}" --preset "${PRESET_NAME}"
    DEPENDS "test-app"
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    VERBATIM
    USES_TERMINAL
)

add_custom_target("cmake-install" ALL
    COMMAND "${CMAKE_COMMAND}" --install "${PROJECT_BINARY_DIR}" --prefix "${CMAKE_INSTALL_PREFIX}"
    DEPENDS ${TARGET_CMAKE_INSTALL_DEPENDS_ON}
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    VERBATIM
    USES_TERMINAL
)

add_custom_target("conan-export"
    COMMAND "${CONAN_COMMAND}"
            export-pkg
            --force
            "${PROJECT_SOURCE_DIR}/conanfile.py"
            #"gitlab-group+gitlab-sub-group+${PROJECT_NAME}/stable"
            --package-folder
            "${CMAKE_INSTALL_PREFIX}"
            ${CONAN_SETTINGS}
            ${CONAN_OPTIONS}
    DEPENDS "${CMAKE_INSTALL_PREFIX}/cmake/${PROJECT_NAME}-config.cmake"
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    VERBATIM
    USES_TERMINAL
)

set_target_names(ALL_TARGETS "${CMAKE_CURRENT_SOURCE_DIR}")
message("all-targets:")
foreach(t IN LISTS ALL_TARGETS)
    message("  ${t}")
endforeach()
