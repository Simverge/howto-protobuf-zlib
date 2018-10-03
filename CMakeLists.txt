cmake_minimum_required(VERSION 3.8)

project(howto-protobuf-zlib)

# Use C11 and C++11 standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

macro(PROCESS_DEPENDENCY NAME REPOSITORY TAG SOURCE_SUBDIR)

	set(DEPENDENCY_NAME ${NAME})
	set(DEPENDENCY_REPOSITORY ${REPOSITORY})
	set(DEPENDENCY_TAG ${TAG})
	set(DEPENDENCY_CMAKE_ARGS ${ARGN})

	set(DEPENDENCY_DOWNLOAD_PROJECT "${DEPENDENCY_NAME}-download")
	set(DEPENDENCY_SOURCE_PROJECT "${DEPENDENCY_NAME}-src")

	find_path(DEPENDENCY_DOWNLOAD_DIR cmake_install.cmake
		HINTS ${CMAKE_BINARY_DIR}/${DEPENDENCY_DOWNLOAD_PROJECT}
		NO_DEFAULT_PATH
		NO_CMAKE_PATH
		NO_CMAKE_ENVIRONMENT_PATH
		NO_SYSTEM_ENVIRONMENT_PATH
	)

	set(DEPENDENCY_SOURCE_DIR "${CMAKE_BINARY_DIR}/${DEPENDENCY_SOURCE_PROJECT}")

	if(NOT DEPENDENCY_DOWNLOAD_DIR OR NOT EXISTS ${DEPENDENCY_SOURCE_DIR})
		# Configure a project to download the dependency
		set(DEPENDENCY_DOWNLOAD_DIR "${CMAKE_BINARY_DIR}/${DEPENDENCY_DOWNLOAD_PROJECT}")

		configure_file(CMakeLists.txt.dep.in ${DEPENDENCY_DOWNLOAD_DIR}/CMakeLists.txt)

		execute_process(
			COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
				RESULT_VARIABLE ${DEPENDENCY_NAME}_CMAKE_ERROR
				WORKING_DIRECTORY ${DEPENDENCY_DOWNLOAD_DIR}
			)

		if(${DEPENDENCY_NAME}_CMAKE_ERROR)
			message(FATAL_ERROR "Could not configure download for ${DEPENDENCY_NAME} dependency: ${${DEPENDENCY_NAME}_CMAKE_ERROR}")
		endif()

		# Build the dependency download project to retrieve the dependency
		execute_process(
			COMMAND ${CMAKE_COMMAND} --build .
			RESULT_VARIABLE ${DEPENDENCY_NAME}_CMAKE_ERROR
			WORKING_DIRECTORY ${DEPENDENCY_DOWNLOAD_DIR}
		)

		if(${DEPENDENCY_NAME}_CMAKE_ERROR)
			message(FATAL_ERROR "Could not complete download for ${DEPENDENCY_NAME} dependency: ${${DEPENDENCY_NAME}_CMAKE_ERROR}")
		endif()
	endif()

	# Configure the dependency source project
	execute_process(
		COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" ${DEPENDENCY_CMAKE_ARGS} ${DEPENDENCY_SOURCE_DIR}/${SOURCE_SUBDIR}
		RESULT_VARIABLE ${DEPENDENCY_NAME}_CMAKE_ERROR
		WORKING_DIRECTORY ${DEPENDENCY_SOURCE_DIR}
	)

	if(${DEPENDENCY_NAME}_CMAKE_ERROR)
		message(FATAL_ERROR "Could not configure project for ${DEPENDENCY_NAME} dependency: ${${DEPENDENCY_NAME}_CMAKE_ERROR}")
	endif()

	# Build and install the dependency source project
	execute_process(
		COMMAND ${CMAKE_COMMAND} --build . --target install --config ${CMAKE_BUILD_TYPE}
		RESULT_VARIABLE ${DEPENDENCY_NAME}_CMAKE_ERROR
		WORKING_DIRECTORY ${DEPENDENCY_SOURCE_DIR}
	)

	if(${DEPENDENCY_NAME}_CMAKE_ERROR)
		message(FATAL_ERROR "Could not build and install project for ${DEPENDENCY_NAME} dependency: ${${DEPENDENCY_NAME}_CMAKE_ERROR}")
	endif()

	find_package(${NAME} REQUIRED)
endmacro()

PROCESS_DEPENDENCY(ZLIB https://github.com/madler/zlib.git v1.2.8 "."
	"-DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}"
)

PROCESS_DEPENDENCY(Protobuf https://github.com/protocolbuffers/protobuf.git v3.5.1 "cmake"
   "-Dprotobuf_BUILD_SHARED_LIBS=ON"
   "-Dprotobuf_BUILD_TESTS=OFF"
   "-Dprotobuf_WITH_ZLIB=ON"
   "-Dprotobuf_DEBUG_POSTFIX=d"
   "-Dprotobuf_VERBOSE=ON"
   "-Dprotobuf_MSVC_STATIC_RUNTIME=OFF"
   "-DCMAKE_DEBUG_POSTFIX=d"
   "-DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}"
   "-DZLIB_ROOT=${CMAKE_BINARY_DIR}"
)
