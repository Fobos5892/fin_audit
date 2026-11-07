cmake_minimum_required(VERSION 3.21)

project(FamilyBudget
    VERSION 1.0.0
    DESCRIPTION "Семейный Бюджет — Qt 6 / QML"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Allow Qt Creator / env to inject Qt path without hardcoding in every kit.
if(DEFINED ENV{CMAKE_PREFIX_PATH} AND NOT CMAKE_PREFIX_PATH)
    set(CMAKE_PREFIX_PATH "$ENV{CMAKE_PREFIX_PATH}" CACHE PATH "Qt prefix" FORCE)
endif()
if(DEFINED ENV{Qt6_DIR} AND NOT Qt6_DIR)
    set(Qt6_DIR "$ENV{Qt6_DIR}" CACHE PATH "Qt6Config.cmake dir" FORCE)
endif()

find_package(Qt6 6.5 REQUIRED COMPONENTS
    Core
    Gui
    Qml
    Quick
    QuickControls2
    Sql
    Concurrent
    Test
)

qt_standard_project_setup(REQUIRES 6.5)

# Auto-pick new sources under src/ (except main.cpp). CONFIGURE_DEPENDS
# re-runs CMake when files are added/removed.
file(GLOB_RECURSE FAMILYBUDGET_CORE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)
list(FILTER FAMILYBUDGET_CORE_SOURCES EXCLUDE REGEX ".*/main\\.cpp$")

add_library(FamilyBudgetCore STATIC
    ${FAMILYBUDGET_CORE_SOURCES}
    resources.qrc
)

target_include_directories(FamilyBudgetCore
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(FamilyBudgetCore
    PUBLIC
        Qt6::Core
        Qt6::Gui
        Qt6::Qml
        Qt6::Quick
        Qt6::QuickControls2
        Qt6::Sql
        Qt6::Concurrent
)

qt_add_executable(FamilyBudget
    src/main.cpp
)

target_link_libraries(FamilyBudget
    PRIVATE
        FamilyBudgetCore
)

set_target_properties(FamilyBudget PROPERTIES
    WIN32_EXECUTABLE TRUE
    OUTPUT_NAME "FamilyBudget"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

# Help Qt Creator Run configuration find QML/plugins next to the exe after windeployqt.
if(WIN32)
    add_custom_command(TARGET FamilyBudget POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Build OK: $<TARGET_FILE:FamilyBudget>"
        VERBATIM
    )
endif()

enable_testing()
add_subdirectory(tests)

message(STATUS "FamilyBudget Qt6: ${Qt6_VERSION}")
message(STATUS "FamilyBudget prefix: ${CMAKE_PREFIX_PATH}")
