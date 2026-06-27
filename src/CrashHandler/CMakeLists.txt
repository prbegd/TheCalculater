 # Copyright © 2026 Cai Yaoxing
 # 
 # This file is part of TheCalculater.
 # TheCalculater is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 # TheCalculater is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 # You should have received a copy of the GNU General Public License along with TheCalculater. If not, see <https://www.gnu.org/licenses/>.
qt_add_executable(CrashHandler
    MANUAL_FINALIZATION
    crash_handler_dialog.cpp
    crash_handler_dialog.hpp
    crash_handler_dialog.ui
    main.cpp
)

target_compile_options(CrashHandler PRIVATE -fexceptions -frtti -fexec-charset=UTF-8 -finput-charset=UTF-8 -fno-omit-frame-pointer -g)
if(WIN32)
    target_compile_options(CrashHandler PRIVATE -Wa,-mbig-obj -fseh-exceptions)
endif()

target_include_directories(CrashHandler PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(CrashHandler PRIVATE ${CMAKE_BINARY_DIR}/include)

target_link_libraries(CrashHandler PRIVATE TheCalculaterCommon TheCalculaterThirdPartyCore TheCalculaterThirdPartyExtra)

set_target_properties(CrashHandler PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

qt_finalize_executable(CrashHandler)