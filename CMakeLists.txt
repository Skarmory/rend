cmake_minimum_required(VERSION 4.2.0)

project(rend
	VERSION 0.1
	LANGUAGES CXX
)

find_package(Vulkan REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS src/*.cpp)
file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS include/*.h)
set(ALL_FILES ${SRC_FILES} ${HEADER_FILES})

add_library(rend SHARED ${ALL_FILES})
add_library(rend::rend ALIAS rend)

target_compile_features(rend PUBLIC cxx_std_20)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${ALL_FILES})

target_include_directories(
	rend
		PUBLIC
			$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
			$<INSTALL_INTERFACE:include>
)

target_link_libraries(rend
	PUBLIC 
		Vulkan::Vulkan
		#Vulkan::Headers
	PRIVATE
		glfw
)

target_compile_definitions(rend
	PRIVATE
		GLFW_WINDOW
		REND_BUILD
		$<$<CONFIG:Debug>:DEBUG>
)

# -----------------------------------------------------------------------------
# Installation
# -----------------------------------------------------------------------------

include(GNUInstallDirs)

install(
	TARGETS rend
	EXPORT rendTargets
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(
	DIRECTORY include/
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
	EXPORT rendTargets
	FILE rendTargets.cmake
	NAMESPACE rend::
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/rend
)

# -----------------------------------------------------------------------------
# CMake package
# -----------------------------------------------------------------------------
include(CMakePackageConfigHelpers)

configure_package_config_file(
	${CMAKE_CURRENT_SOURCE_DIR}/cmake/rendConfig.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/rendConfig.cmake
	INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/rend
)

write_basic_package_version_file(
	${CMAKE_CURRENT_BINARY_DIR}/rendConfigVersion.cmake
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY SameMajorVersion
)

install(
	FILES
		${CMAKE_CURRENT_BINARY_DIR}/rendConfig.cmake
		${CMAKE_CURRENT_BINARY_DIR}/rendConfigVersion.cmake
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/rend
)