include_directories(
	${PROTOBUF_INCLUDE_DIR}
	${ZLIB_INCLUDE_DIR}
)

message(STATUS "PROTOBUF_INCLUDE_DIR: ${PROTOBUF_INCLUDE_DIR}")

# Protobuf examples
# Copy the official address book example from the Protobuf repository.
# Replace the inclusion of "google/protobuf/timestamp.proto" with
# a timestamp.proto in the current source directory

file(READ ${CMAKE_BINARY_DIR}/Protobuf/examples/addressbook.proto ORIGINAL_ADDRESSBOOK_PROTO)
string(REPLACE "import \"google/protobuf/" "import \""
	MODIFIED_ADDRESSBOOK_PROTO "${ORIGINAL_ADDRESSBOOK_PROTO}")
file(WRITE ${CMAKE_CURRENT_SOURCE_DIR}/addressbook.proto "${MODIFIED_ADDRESSBOOK_PROTO}")

file(COPY ${CMAKE_BINARY_DIR}/include/google/protobuf/timestamp.proto DESTINATION ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB PROTOBUF_PROTOS *.proto)

if (WIN32)
	list(APPEND PROTOBUF_DEFINITIONS -DPROTOBUF_USE_DLLS)
endif ()

protobuf_generate_cpp(PROTOBUF_SOURCES PROTOBUF_HEADERS ${PROTOBUF_PROTOS})

# Writer example

set(WRITER_TARGET protobuf_zlib_write_example)

set(WRITER_SOURCES
	${PROTOBUF_SOURCES}
	protobuf_zlib_write_example.cpp
)

set(WRITER_HEADERS
	${PROTOBUF_HEADERS}
)

set(WRITER_DEPENDENCIES
	${PROTOBUF_LIBRARIES}
	${ZLIB_LIBRARIES}
)

add_definitions(${PROTOBUF_DEFINITIONS})
add_executable(${WRITER_TARGET} ${WRITER_SOURCES} ${WRITER_HEADERS})
target_link_libraries(${WRITER_TARGET} ${WRITER_DEPENDENCIES})

# Reader example

set(READER_TARGET protobuf_zlib_read_example)

set(READER_SOURCES
	${PROTOBUF_SOURCES}
	protobuf_zlib_read_example.cpp
)

set(READER_HEADERS
	${PROTOBUF_HEADERS}
)

set(READER_DEPENDENCIES
	${PROTOBUF_LIBRARIES}
	${ZLIB_LIBRARIES}
)

add_definitions(${PROTOBUF_DEFINITIONS})
add_executable(${READER_TARGET} ${READER_SOURCES} ${READER_HEADERS})
target_link_libraries(${READER_TARGET} ${READER_DEPENDENCIES})

# Installation
install(TARGETS ${WRITER_TARGET} ${READER_TARGET} DESTINATION bin)