cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

if(NOT "${CMAKE_SCRIPT_MODE_FILE}" STREQUAL "")
    message(FATAL_ERROR "Unsupported in script mode '${CMAKE_CURRENT_LIST_FILE}'!")
endif()

function(vscode)
    set(currentFunctionName "${CMAKE_CURRENT_FUNCTION}")
    set(options)
    set(oneValueKeywords
        "SETTINGS_FILE"
        "C_CPP_PROPERTIES_FILE"
        "TASKS_FILE"
        "LAUNCH_FILE"
    )
    set(multiValueKeywords)

    cmake_parse_arguments("${currentFunctionName}" "${options}" "${oneValueKeywords}" "${multiValueKeywords}" "${ARGN}")

    # Generate ".vscode/settings.json"
    if(NOT "${${currentFunctionName}_SETTINGS_FILE}" STREQUAL "")
        if(EXISTS "${${currentFunctionName}_SETTINGS_FILE}")
            file(READ "${${currentFunctionName}_SETTINGS_FILE}" settings)
            if("${settings}" STREQUAL "")
                set(settings "{}")
            endif()
        else()
            set(settings "{}")
        endif()

        # Get keys
        set(settingsKeys)
        string(JSON listLength LENGTH "${settings}")
        if("${listLength}" GREATER "0")
            math(EXPR maxIndex "${listLength} - 1")
            foreach(i RANGE "0" "${maxIndex}")
                string(JSON k MEMBER "${settings}" "${i}")
                list(APPEND settingsKeys "${k}")
            endforeach()
        endif()

        # Set items
        set(k "C_Cpp.errorSquiggles")
        if(NOT "${k}" IN_LIST "settingsKeys")
            string(JSON settings SET "${settings}" "${k}" "\"enabled\"")
        endif()
        set(k "C_Cpp.intelliSenseEngine")
        if(NOT "${k}" IN_LIST "settingsKeys")
            string(JSON settings SET "${settings}" "${k}" "\"default\"")
        endif()
        set(k "C_Cpp.intelliSenseEngineFallback")
        if(NOT "${k}" IN_LIST "settingsKeys")
            string(JSON settings SET "${settings}" "${k}" "\"enabled\"")
        endif()
        set(k "files.readonlyFromPermissions")
        if(NOT "${k}" IN_LIST "settingsKeys")
            string(JSON settings SET "${settings}" "${k}" "true")
        endif()
        set(k "files.associations")
        if(NOT "${k}" IN_LIST "settingsKeys")
            string(JSON settings SET "${settings}" "${k}" "{}")
            string(JSON settings SET "${settings}" "${k}" "*.h" "\"c\"")
            string(JSON settings SET "${settings}" "${k}" "*.c" "\"c\"")
            string(JSON settings SET "${settings}" "${k}" "*.hpp" "\"cpp\"")
            string(JSON settings SET "${settings}" "${k}" "*.cpp" "\"cpp\"")
        endif()

        file(WRITE "${${currentFunctionName}_SETTINGS_FILE}" "${settings}")
    endif()

    # Generate ".vscode/c_cpp_properties.json"
    if(NOT "${${currentFunctionName}_C_CPP_PROPERTIES_FILE}" STREQUAL "")
        if(EXISTS "${${currentFunctionName}_C_CPP_PROPERTIES_FILE}")
            file(READ "${${currentFunctionName}_C_CPP_PROPERTIES_FILE}" cCppProperties)
            if("${cCppProperties}" STREQUAL "")
                string(JSON cCppProperties SET "{}" "version" "4")
                string(JSON cCppProperties SET "${cCppProperties}" "configurations" "[]")
            endif()
        else()
            string(JSON cCppProperties SET "{}" "version" "4")
            string(JSON cCppProperties SET "${cCppProperties}" "configurations" "[]")
        endif()

        string(REPLACE "." ";" values "${PRESET_NAME}")
        list(GET "values" "2" entryName)

        # Generate ".vscode/c_cpp_properties.json" content
        string(JSON entry SET "{}" "name" "\"${entryName}\"")
        string(JSON entry SET "${entry}" "compilerPath" "\"${CMAKE_CXX_COMPILER}\"")
        set(v "${PROJECT_BINARY_DIR}/compile_commands.json")
        cmake_path(RELATIVE_PATH v BASE_DIRECTORY "${PROJECT_SOURCE_DIR}")
        string(JSON entry SET "${entry}" "compileCommands" "\"${v}\"")
        if("${entryName}" STREQUAL "gcc-arm")
            set(v "gcc-arm")
        elseif("${entryName}" STREQUAL "gcc")
            set(v "gcc-x64")
        elseif("${entryName}" STREQUAL "msvc")
            set(v "msvc-x64")
        endif()
        string(JSON entry SET "${entry}" "intelliSenseMode" "\"${v}\"")
        string(JSON entry SET "${entry}" "cStandard" "\"c${CMAKE_C_STANDARD}\"")
        string(JSON entry SET "${entry}" "cppStandard" "\"c++${CMAKE_CXX_STANDARD}\"")
        string(JSON entry SET "${entry}" "configurationProvider" "\"ms-vscode.cmake-tools\"")
        string(JSON entry SET "${entry}" "mergeConfigurations" "true")

        # Find index
        string(JSON entryIndex LENGTH "${cCppProperties}" "configurations")
        if("${entryIndex}" GREATER "0")
            math(EXPR maxIndex "${entryIndex} - 1")
           foreach(i RANGE "0" "${maxIndex}")
                string(JSON v GET "${cCppProperties}" "configurations" "${i}")
                string(JSON vName GET "${v}" "name")
                if("${vName}" STREQUAL "${entryName}")
                    set(entryIndex "${i}")
                    break()
                endif()
            endforeach()
        endif()

        # Set item
        string(JSON cCppProperties SET "${cCppProperties}" "configurations" "${entryIndex}" "${entry}")

        file(WRITE "${${currentFunctionName}_C_CPP_PROPERTIES_FILE}" "${cCppProperties}")
    endif()

    # Generate ".vscode/tasks.json"
    if(NOT "${${currentFunctionName}_TASKS_FILE}" STREQUAL "")
        if(EXISTS "${${currentFunctionName}_TASKS_FILE}")
            file(READ "${${currentFunctionName}_TASKS_FILE}" tasks)
            if("${tasks}" STREQUAL "")
                string(JSON tasks SET "{}" "version" "\"2.0.0\"")
                string(JSON tasks SET "${tasks}" "tasks" "[]")
            endif()
        else()
            string(JSON tasks SET "{}" "version" "\"2.0.0\"")
            string(JSON tasks SET "${tasks}" "tasks" "[]")
        endif()

        set(entryName "CMake.Build.${PROJECT_NAME}")

        # Generate ".vscode/tasks.json" content
        string(JSON entry SET "{}" "label" "\"${entryName}\"")
        string(JSON entry SET "${entry}" "type" "\"cmake\"")
        string(JSON entry SET "${entry}" "options" "{}")
        string(JSON entry SET "${entry}" "options" "cwd" "\"\${workspaceFolder}\"")
        string(JSON entry SET "${entry}" "group" "{}")
        string(JSON entry SET "${entry}" "group" "kind" "\"build\"")
        string(JSON entry SET "${entry}" "group" "isDefault" "true")
        string(JSON entry SET "${entry}" "problemMatcher" "[]")
        string(JSON entry SET "${entry}" "preset" "\"\${command:cmake.activeConfigurePresetName}\"")
        string(JSON entry SET "${entry}" "command" "\"build\"")
        string(JSON entry SET "${entry}" "targets" "[]")
        string(JSON entry SET "${entry}" "targets" "0" "\"${PROJECT_NAME}\"")

        # Find index
        string(JSON entryIndex LENGTH "${tasks}" "tasks")
        if("${entryIndex}" GREATER "0")
            math(EXPR maxIndex "${entryIndex} - 1")
            foreach(i RANGE "0" "${maxIndex}")
                string(JSON v GET "${tasks}" "tasks" "${i}")
                string(JSON vLabel GET "${v}" "label")
                if("${vLabel}" STREQUAL "${entryName}")
                    set(entryIndex "${i}")
                    break()
                endif()
            endforeach()
        endif()

        # Set item
        string(JSON tasks SET "${tasks}" "tasks" "${entryIndex}" "${entry}")

        set(entryName "CMake.Build.test-app")

        # Generate entry
        string(JSON entry SET "{}" "label" "\"${entryName}\"")
        string(JSON entry SET "${entry}" "type" "\"cmake\"")
        string(JSON entry SET "${entry}" "options" "{}")
        string(JSON entry SET "${entry}" "options" "cwd" "\"\${workspaceFolder}\"")
        string(JSON entry SET "${entry}" "group" "{}")
        string(JSON entry SET "${entry}" "group" "kind" "\"build\"")
        string(JSON entry SET "${entry}" "group" "isDefault" "true")
        string(JSON entry SET "${entry}" "problemMatcher" "[]")
        string(JSON entry SET "${entry}" "preset" "\"\${command:cmake.activeConfigurePresetName}\"")
        string(JSON entry SET "${entry}" "command" "\"build\"")
        string(JSON entry SET "${entry}" "targets" "[]")
        string(JSON entry SET "${entry}" "targets" "0" "\"test-app\"")

        # Find index
        string(JSON entryIndex LENGTH "${tasks}" "tasks")
        if("${entryIndex}" GREATER "0")
            math(EXPR maxIndex "${entryIndex} - 1")
            foreach(i RANGE "0" "${maxIndex}")
                string(JSON v GET "${tasks}" "tasks" "${i}")
                string(JSON vLabel GET "${v}" "label")
                if("${vLabel}" STREQUAL "${entryName}")
                    set(entryIndex "${i}")
                    break()
                endif()
            endforeach()
        endif()

        # Set item
        string(JSON tasks SET "${tasks}" "tasks" "${entryIndex}" "${entry}")

        file(WRITE "${${currentFunctionName}_TASKS_FILE}" "${tasks}")
    endif()

    # Generate ".vscode/launch.json"
    if(NOT "${${currentFunctionName}_LAUNCH_FILE}" STREQUAL "")
        if(EXISTS "${${currentFunctionName}_LAUNCH_FILE}")
            file(READ "${${currentFunctionName}_LAUNCH_FILE}" launch)
            if("${launch}" STREQUAL "")
                string(JSON launch SET "{}" "version" "\"0.2.0\"")
                string(JSON launch SET "${launch}" "configurations" "[]")
            endif()
        else()
            string(JSON launch SET "{}" "version" "\"0.2.0\"")
            string(JSON launch SET "${launch}" "configurations" "[]")
        endif()

        set(entryName "Launch.test-app")

        # Generate entry
        string(JSON entry SET "{}" "name" "\"${entryName}\"")
        string(JSON entry SET "${entry}" "preLaunchTask" "\"CMake.Build.test-app\"")
        string(JSON entry SET "${entry}" "type" "\"cppvsdbg\"")
        string(JSON entry SET "${entry}" "request" "\"launch\"")
        string(JSON entry SET "${entry}" "program" "\"${PROJECT_BINARY_DIR}/test/bin/test-app.exe\"")
        string(JSON entry SET "${entry}" "args" "[]")
        string(JSON entry SET "${entry}" "stopAtEntry" "false")
        string(JSON entry SET "${entry}" "cwd" "\"${PROJECT_BINARY_DIR}/test/bin\"")
        string(JSON entry SET "${entry}" "console" "\"externalTerminal\"")
        string(JSON entry SET "${entry}" "environment" "[]")
        string(JSON entry SET "${entry}" "environment" "0" "{}")
        string(JSON entry SET "${entry}" "environment" "0" "name" "\"PATH\"")
        string(JSON entry SET "${entry}" "environment" "0" "value" "\"${PROJECT_BINARY_DIR}/test/bin\"")

        # Find index
        string(JSON entryIndex LENGTH "${launch}" "configurations")
        if("${entryIndex}" GREATER "0")
            math(EXPR maxIndex "${entryIndex} - 1")
            foreach(i RANGE "0" "${maxIndex}")
                string(JSON v GET "${launch}" "configurations" "${i}")
                string(JSON vName GET "${v}" "name")
                if("${vName}" STREQUAL "${entryName}")
                    set(entryIndex "${i}")
                    break()
                endif()
            endforeach()
        endif()

        # Set item
        string(JSON launch SET "${launch}" "configurations" "${entryIndex}" "${entry}")

        # Extract test names
        set(testCaseNames)
        if(EXISTS "${PROJECT_BINARY_DIR}/test/bin/test-app.exe")
            if(EXISTS "${PROJECT_BINARY_DIR}/test/bin/gtest_list_tests.json")
                file(REMOVE "${PROJECT_BINARY_DIR}/test/bin/gtest_list_tests.json")
            endif()
            execute_process(
                COMMAND "${PROJECT_BINARY_DIR}/test/bin/test-app.exe" "--gtest_list_tests" "--gtest_output=json:gtest_list_tests.json"
                WORKING_DIRECTORY "${PROJECT_BINARY_DIR}/test/bin"
                OUTPUT_QUIET
                TIMEOUT "3"
                RESULT_VARIABLE "gtestListTestsResult"
            )
            if("${gtestListTestsResult}" EQUAL "0" AND EXISTS "${PROJECT_BINARY_DIR}/test/bin/gtest_list_tests.json")
                file(READ "${PROJECT_BINARY_DIR}/test/bin/gtest_list_tests.json" json)

                string(JSON testGroups GET "${json}" "testsuites")
                string(JSON testGroupsLength LENGTH "${testGroups}")

                if("${testGroupsLength}" GREATER "0")
                    math(EXPR testGroupMaxIndex "${testGroupsLength} - 1")
                    foreach(testGroupIndex RANGE "0" "${testGroupMaxIndex}")

                        string(JSON testGroup GET "${testGroups}" "${testGroupIndex}")
                        string(JSON testGroupName GET "${testGroup}" "name")
                        string(JSON testCases GET "${testGroup}" "testsuite")
                        string(JSON testCasesLength LENGTH "${testCases}")

                        if("${testCasesLength}" GREATER "0")
                            math(EXPR testCaseMaxIndex "${testCasesLength} - 1")
                            foreach(testCaseIndex RANGE "0" "${testCaseMaxIndex}")

                                string(JSON testCase GET "${testCases}" "${testCaseIndex}")
                                string(JSON testCaseName GET "${testCase}" "name")

                                if(NOT "${testGroupName}.*" IN_LIST "testCaseNames")
                                    list(APPEND testCaseNames "${testGroupName}.*")
                                endif()
                                if(NOT "${testGroupName}.${testCaseName}" IN_LIST "testCaseNames")
                                    list(APPEND testCaseNames "${testGroupName}.${testCaseName}")
                                endif()

                            endforeach()
                        endif()

                    endforeach()
                endif()
            endif()
        endif()

        # Generate test entries
        if(NOT "${testCaseNames}" STREQUAL "")
            foreach(i IN LISTS "testCaseNames")

                set(entryName "Launch.${i}")

                # Generate entry
                string(JSON entry SET "{}" "name" "\"${entryName}\"")
                string(JSON entry SET "${entry}" "preLaunchTask" "\"CMake.Build.test-app\"")
                string(JSON entry SET "${entry}" "type" "\"cppvsdbg\"")
                string(JSON entry SET "${entry}" "request" "\"launch\"")
                string(JSON entry SET "${entry}" "program" "\"${PROJECT_BINARY_DIR}/test/bin/test-app.exe\"")
                string(JSON entry SET "${entry}" "args" "[]")
                string(JSON entry SET "${entry}" "args" "0" "\"--gtest_filter=${i}\"")
                string(JSON entry SET "${entry}" "stopAtEntry" "false")
                string(JSON entry SET "${entry}" "cwd" "\"${PROJECT_BINARY_DIR}/test/bin\"")
                string(JSON entry SET "${entry}" "console" "\"externalTerminal\"")
                string(JSON entry SET "${entry}" "environment" "[]")
                string(JSON entry SET "${entry}" "environment" "0" "{}")
                string(JSON entry SET "${entry}" "environment" "0" "name" "\"PATH\"")
                string(JSON entry SET "${entry}" "environment" "0" "value" "\"${PROJECT_BINARY_DIR}/test/bin\"")

                # Find index
                string(JSON entryIndex LENGTH "${launch}" "configurations")
                if("${entryIndex}" GREATER "0")
                    math(EXPR maxIndex "${entryIndex} - 1")
                    foreach(i RANGE "0" "${maxIndex}")
                        string(JSON v GET "${launch}" "configurations" "${i}")
                        string(JSON vName GET "${v}" "name")
                        if("${vName}" STREQUAL "${entryName}")
                            set(entryIndex "${i}")
                            break()
                        endif()
                    endforeach()
                endif()

                # Set item
                string(JSON launch SET "${launch}" "configurations" "${entryIndex}" "${entry}")

            endforeach()
        endif()

        file(WRITE "${${currentFunctionName}_LAUNCH_FILE}" "${launch}")
    endif()
endfunction()
