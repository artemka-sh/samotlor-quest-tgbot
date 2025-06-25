#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "TgBot::TgBot" for configuration "Debug"
set_property(TARGET TgBot::TgBot APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(TgBot::TgBot PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib64/libTgBot.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS TgBot::TgBot )
list(APPEND _IMPORT_CHECK_FILES_FOR_TgBot::TgBot "${_IMPORT_PREFIX}/lib64/libTgBot.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
