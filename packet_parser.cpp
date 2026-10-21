#include "packet_parser.h"
#include <arpa/inet.h>
#include <iostream>
#include <cstdint> //For using std::uint8_t
#include <string>
using namespace std;

std::optional<PacketInfo> PacketParser::parse(const u_char* bytes, int caplen, int len) {
	constexpr std::size_t eth = 14;
	if (caplen < eth + 20) {
		return std::nullopt;
	}

	const std::uint8_t first = bytes[eth];
	const std::uint8_t version = first >> 4; // Top 4 bits
	const std::size_t ip_len = (first & 0x0F) * 4; // Bottom 4 bits * 4 bytes

	if (version != 4 || ip_len < 20 || caplen < eth + ip_len) {
		//This verifies IPv4 versin and header bounds
		return std::nullopt;
	}
	const std::size_t transport = eth + ip_len;

	const std::uint8_t protocol_num = bytes[eth + 9];
	if (protocol_num != 6 && protocol_num != 17) {
		//If you look at the notes on each part IPv4 packet, TCP = 6, and UDP = 17
		//That is what we are looking for, so if protocol_num does not equal either
		//We don't want it. 
		return std::nullopt;
	}

	//defining src and dst, and putting in the IP address.
	char src[INET_ADDRSTRLEN]{};
	char dst[INET_ADDRSTRLEN]{};
	//Since we defined each of the packet infos as string we need to do:
	std::string source_ip = std::to_string(bytes[eth + 12]) + "." +
		std::to_string(bytes[eth + 13]) + "." +
		std::to_string(bytes[eth + 14]) + "." +
		std::to_string(bytes[eth + 15]);
	std::string destination_ip = std::to_string(bytes[eth + 16]) + "." +
		std::to_string(bytes[eth + 17]) + "." +
		std::to_string(bytes[eth + 18]) + "." +
		std::to_string(bytes[eth + 19]);

	//Because UDP and TCP requires a length of 6/17 this makes sure that only UDP and TCP are captured
	//UDP and TCP are the only ones required for capture for the assignment. 
	if (protocol_num == 6 && caplen < transport + 20) {
		return std::nullopt;
	}
	if (protocol_num == 17 && caplen < transport + 8) {
		return std::nullopt;
	}
	//This is for the source port, it takes the high bytes and low bytes and combines them
	std::uint8_t high_byte_src = bytes[transport];
	std::uint8_t low_byte_src = bytes[transport + 1];
	std::uint16_t src_port = (high_byte_src * 256) + low_byte_src;

	//This is for the destination port, it takes the high bytes and low bytes and combines them
	std::uint8_t high_byte_dst = bytes[transport + 2];
	std::uint8_t low_byte_dst = bytes[transport + 3];
	std::uint16_t dst_port = (high_byte_dst * 256) + low_byte_dst;

	//Think of the high byte as the tens place, adn the low byte as the ones place.
	//The 256 is to offset the placement of the high_byte. 
	//Thinking of in terms of trying to get the high and low place of 94:
	// (9*10) + 4, where 9 is the heigh_byte and 4 is the low_byte


	//The items below use the dataframe that is provided by packet_info
	PacketInfo info;
	info.source_ip = source_ip;
	info.destination_ip = destination_ip;
	info.source_port = src_port;
	info.destination_port = dst_port;
	if (protocol_num == 6) {
		info.protocol = TransportProtocol::TCP;
	}
	else {
		info.protocol = TransportProtocol::UDP;
	}
	info.wire_bytes = len;

	return info;

	/*Eacher IPv4 is going to be structured as such:
	Version (4 bits): for IPv4 packets(the ones you will be using?

	IHL / Header Length (4 bits): Length of the header in 32-bit words. Minimum is 5 (20 bytes), maximum is 15 (60 bytes) .

	Type of Service / DS Field (8 bits): Specifies QoS parameters like delay, throughput, reliability, and ECN for congestion notification .

	Total length (16 bits): for engtire packet size, which can rang efrom 20 to 65,535 bytes

	Identification (16 bits): Unique ID for reassembling fragmented packets.

	Flags (3 bits): Control fragmentation — Reserved, DF (Don’t Fragment), MF (More Fragments).

	Fragment Offset (13 bits): Position of a fragment relative to the start of the original packet.

	Time to Live (8 bits): Hop limit; decremented by each router to prevent infinite loops.

	Protocol (8 bits): Indicates the encapsulated protocol (e.g., TCP=6, UDP=17, ICMP=1).

	Header Checksum (16 bits): Error-checking for the header only; recalculated at each hop.

	Source Address (32 bits): IPv4 address of the sender.

	Destination Address (32 bits): IPv4 address of the receiver.

	Options (variable): Optional control information like source routing; padded to 32-bit boundaries .

	This comes from a CoPilot search that quotes computernetworkingnotes.com/networking-tutorials/ipv4-header-structure-and-fields-explained.html
	 */
	
}