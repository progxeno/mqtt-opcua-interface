cmake_minimum_required(VERSION 3.5)

get_filename_component(DEV_ROOT "${CMAKE_CURRENT_SOURCE_DIR}" ABSOLUTE)

set(PROJECT_ROOT "${DEV_ROOT}/")

set(SUBMODULE_ROOT "${DEV_ROOT}/../../../")

set(PROJECT_NAME "Masterarbeit")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

set(MAIN_SRCS ${PROJECT_ROOT}/main/app_main.c)

set(EXTRA_COMPONENT_DIRS "${EXTRA_COMPONENT_DIRS} ${SUBMODULE_ROOT}")
set(BUILD_COMPONENTS "${BUILD_COMPONENTS} espmqtt")

ua_generate_nodeset(
    NAME "nodeset"
    FILE "${PROJECT_ROOT}/src/nodeset.xml"
    DEPENDS_TYPES "UA_TYPES"
    DEPENDS_NS    "${UA_FILE_NS0}"
)

project(${PROJECT_NAME})

