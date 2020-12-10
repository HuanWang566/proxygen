# Copyright (c) Facebook, Inc. and its affiliates.
# All rights reserved.
#
# This source code is licensed under the BSD-style license found in the
# LICENSE file in the root directory of this source tree.

add_library(
    proxygenhttpserver
    RequestHandlerAdaptor.cpp
    SignalHandler.cpp
    HTTPServerAcceptor.cpp
    HTTPServer.cpp
)
target_compile_options(
    proxygenhttpserver
    PRIVATE
        ${_PROXYGEN_COMMON_COMPILE_OPTIONS}
)
if (BUILD_SHARED_LIBS)
    set_property(TARGET proxygenhttpserver PROPERTY POSITION_INDEPENDENT_CODE ON)
endif()

target_link_libraries(
    proxygenhttpserver
    PUBLIC
        proxygen
)
install(
    TARGETS proxygenhttpserver
    EXPORT proxygen-exports
    DESTINATION lib
)

add_executable(proxygen_push
    samples/push/PushServer.cpp
    samples/push/PushRequestHandler.cpp
)
target_compile_options(
    proxygen_push
    PRIVATE
        ${_PROXYGEN_COMMON_COMPILE_OPTIONS}
)
target_link_libraries(
    proxygen_push
    PUBLIC
        proxygen
        proxygenhttpserver
)
install(
    TARGETS proxygen_push
    EXPORT proxygen-exports
    DESTINATION bin
)

add_executable(proxygen_proxy
    samples/proxy/ProxyServer.cpp
    samples/proxy/ProxyHandler.cpp
)
target_compile_options(
    proxygen_proxy
    PRIVATE
        ${_PROXYGEN_COMMON_COMPILE_OPTIONS}
)
target_link_libraries(
    proxygen_proxy
    PUBLIC
        proxygen
        proxygenhttpserver
)
install(
    TARGETS proxygen_proxy
    EXPORT proxygen-exports
    DESTINATION bin
)

add_executable(proxygen_static
    samples/static/StaticServer.cpp
    samples/static/StaticHandler.cpp
)
target_compile_options(
    proxygen_static
    PRIVATE
        ${_PROXYGEN_COMMON_COMPILE_OPTIONS}
)
target_link_libraries(
    proxygen_static
    PUBLIC
        proxygen
        proxygenhttpserver
)
install(
    TARGETS proxygen_static
    EXPORT proxygen-exports
    DESTINATION bin
)

add_executable(proxygen_echo
    samples/echo/EchoServer.cpp
    samples/echo/EchoHandler.cpp
)
target_compile_options(
    proxygen_echo
    PRIVATE
        ${_PROXYGEN_COMMON_COMPILE_OPTIONS}
)
target_link_libraries(
    proxygen_echo
    PUBLIC
        proxygen
        proxygenhttpserver
)
install(
    TARGETS proxygen_echo
    EXPORT proxygen-exports
    DESTINATION bin
)

# begin of control center for demo factory
include_directories(../../../CSerialPort/include)

file(GLOB_RECURSE COMMON_SOURCES ../../../CSerialPort/src/SerialPort.cpp ../../../CSerialPort/src/SerialPortBase.cpp ../../../CSerialPort/src/SerialPortInfo.cpp ../../../CSerialPort/src/SerialPortInfoBase.cpp)

IF (CMAKE_HOST_WIN32)
	file(GLOB_RECURSE OS_ABOUT_SOURCES ../../../CSerialPort/src/SerialPortInfoWinBase.cpp ../../../CSerialPort/src/SerialPortWinBase.cpp)
ELSEIF (CMAKE_HOST_UNIX)
	file(GLOB_RECURSE OS_ABOUT_SOURCES ../../../CSerialPort/src/SerialPortInfoUnixBase.cpp ../../../CSerialPort/src/SerialPortUnixBase.cpp)
ENDIF ()

add_executable(control_center
    samples/control_center/ControlCenterServer.cpp
    samples/control_center/ControlCenterHandler.cpp
    ${COMMON_SOURCES} 
    ${OS_ABOUT_SOURCES}
)


target_compile_options(
    control_center
    PRIVATE
        ${_PROXYGEN_COMMON_COMPILE_OPTIONS}
)
target_link_libraries(
    control_center
    PUBLIC
        proxygen
        proxygenhttpserver
        pthread
)

install(
    TARGETS control_center
    EXPORT proxygen-exports
    DESTINATION bin
)

# end of control center for demo factory

if (BUILD_QUIC)
  add_executable(hq
      samples/hq/main.cpp
      samples/hq/FizzContext.cpp
      samples/hq/HQClient.cpp
      samples/hq/HQLoggerHelper.cpp
      samples/hq/HQServer.cpp
      samples/hq/HQParams.cpp
      samples/hq/SampleHandlers.cpp
      samples/hq/PartiallyReliableCurlClient.cpp
  )
  target_compile_options(
      hq
      PRIVATE
          ${_PROXYGEN_COMMON_COMPILE_OPTIONS}
  )
  target_link_libraries(
      hq
      PUBLIC
          proxygen
          proxygencurl
          proxygenhttpserver
          mvfst::mvfst_transport
          mvfst::mvfst_client
          mvfst::mvfst_server
  )
  install(
      TARGETS hq
      EXPORT proxygen-exports
      DESTINATION bin
  )
endif()

file(
    GLOB_RECURSE PROXYGEN_HTTPSERVER_HEADERS_TOINSTALL
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    *.h
)
list(FILTER PROXYGEN_HTTPSERVER_HEADERS_TOINSTALL EXCLUDE REGEX tests/)
list(FILTER PROXYGEN_HTTPSERVER_HEADERS_TOINSTALL EXCLUDE REGEX Mocks.h)
list(FILTER PROXYGEN_HTTPSERVER_HEADERS_TOINSTALL EXCLUDE REGEX samples/)
foreach(header ${PROXYGEN_HTTPSERVER_HEADERS_TOINSTALL})
    get_filename_component(header_dir ${header} DIRECTORY)
    install(FILES ${header} DESTINATION include/proxygen/httpserver/${header_dir})
endforeach()

add_subdirectory(tests)
add_subdirectory(filters/tests)
#add_subdirectory(samples/echo/test)
