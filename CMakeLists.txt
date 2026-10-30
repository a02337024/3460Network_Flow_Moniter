cmake_minimum_required(VERSION 3.14)
project(PacketSniffer CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(packet_sniffer
    main.cpp
    packet_parser.cpp
)

# Hardcoded paths directly pointing to your Npcap SDK location
set(PCAP_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/npcap-sdk-1.16/Include")
set(PCAP_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/npcap-sdk-1.16/Lib/x64/wpcap.lib")

if(EXISTS "${PCAP_INCLUDE_DIR}" AND EXISTS "${PCAP_LIBRARY}")
    target_include_directories(packet_sniffer PRIVATE ${PCAP_INCLUDE_DIR})
    target_link_libraries(packet_sniffer PRIVATE ${PCAP_LIBRARY})
    message(STATUS "Successfully configured Npcap SDK from local folder.")
else()
    message(FATAL_ERROR "Could not find Npcap SDK at: ${CMAKE_CURRENT_SOURCE_DIR}/npcap-sdk-1.16")
endif()