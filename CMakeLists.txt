cmake_minimum_required(VERSION 3.12)

include("../CMakeShared.txt")

set(PROJ_NAME pr_audio_fmod)

project(${PROJ_NAME} CXX)

add_include_dir(glm)
link_external_library(alsoundsystem)

# FMOD
link_external_library(libsamplerate)

add_include_dir(fmod)
link_external_library(fmod)
link_external_library(fmod_studio)

link_generic_module_libraries()
register_pragma_module(${PROJ_NAME})

add_dependencies(${PROJ_NAME} alsoundsystem)

set_target_properties(pr_audio_fmod PROPERTIES FOLDER modules/audio)

set_property(GLOBAL PROPERTY PRAGMA_MODULE_SKIP_TARGET_PROPERTY_FOLDER 1)
