set (PROTOBUF_PROTOS 
	blob.proto
)

if (WIN32)
	list(APPEND PROTOBUF_DEFINITIONS -DPROTOBUF_USE_DLLS)
endif ()

protobuf_generate_cpp(PROTOBUF_SOURCES PROTOBUF_HEADERS ${PROTOBUF_PROTOS})

set(HEADERS
	${PROTOBUF_HEADERS}
)

set(DEPENDENCIES
	${PROTOBUF_LIBRARIES}
	${ZLIB_LIBRARIES}
)

# Reader example

set(READER_TARGET protobuf_zlib_read_example)

set(READER_SOURCES
	${PROTOBUF_SOURCES}
	protobuf_zlib_read_example.cpp
)

include_directories(
	${CMAKE_BINARY_DIR}
	${PROTOBUF_INCLUDE_DIR}
	${ZLIB_INCLUDE_DIR}
)

add_definitions(${PROTOBUF_DEFINITIONS})
add_executable(${READER_TARGET} ${READER_SOURCES} ${HEADERS})
target_link_libraries(${READER_TARGET} ${DEPENDENCIES})

# Writer example

set(WRITER_TARGET protobuf_zlib_write_example)

set(WRITER_SOURCES
	${PROTOBUF_SOURCES}
	protobuf_zlib_write_example.cpp
)

set(WRITER_HEADERS
	${PROTOBUF_HEADERS}
)

include_directories(
	${CMAKE_BINARY_DIR}
	${PROTOBUF_INCLUDE_DIR}
	${ZLIB_INCLUDE_DIR}
)

add_definitions(${PROTOBUF_DEFINITIONS})
add_executable(${WRITER_TARGET} ${WRITER_SOURCES} ${HEADERS})
target_link_libraries(${WRITER_TARGET} ${DEPENDENCIES})

# Installation
install(TARGETS ${WRITER_TARGET} ${READER_TARGET} DESTINATION ${CMAKE_BINARY_DIR}/bin)
