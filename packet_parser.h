#pragma once //This line of code tells the compiler to only include this header file once. 
#include <optional> //Allows for the creation of a variable for information that may or may not exist
#include <pcap.h>
#include "packet_info.h"

class PacketParser {
public:
	std::optional<PacketInfo> parse(const u_char* bytes, int caplen, int len);
};
/*This class will do: Ethernet -> IPv4 -> TCP/UDP

The main responsibility of the PacketParser is to walk through the ehternet, IPv4, and TCP/UDP layers
while performing strict bounds checks, because things might fail.
*/
