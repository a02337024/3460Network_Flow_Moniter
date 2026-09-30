#include <iostream>
#include <stdexcept>
#include <pcap.h>
#include <memory>
#include "packet_parser.h"
#include "packet_Info.h"
using namespace std;

int main(int argc, char*argv[]) {
	
	if (argc > 2) {
		throw runtime_error("Cannot accept more than 1 argument");
	}
	if (argc < 2) {
		std::cerr << "Error: Must provide a network interface." << std::endl;
		return 1;
	}
	PacketParser parser; //defines the PacketParser object
	
	
	char errbuf[PCAP_ERRBUF_SIZE]{}; //This is an empty character array where array messages will be stored
	//(PCAP_ERRBUF_SIZE is from libpvap and specifies how large errbuf should be)
	std::string input = argv[1];
	std::string interface = " ";
#if defined(_WIN32) || defined(__WIN32__) || defined(__CYGWIN__) || defined(__MSYS__)
	if (input.rfind("\\Device\\NPF_", 0) == 0) {
		interface = input;
	}
	else {
		if (!input.empty() && input.front() == '{') input.erase(0, 1);
		if (!input.empty() && input.back() == '}') input.pop_back();

		interface = "\\Device\\NPF_{" + input + "}";
	}
#else
	interface = input;
#endif
	std::cout << "Opening device: " << interface << std::endl;
	pcap_t* raw = pcap_open_live(interface.c_str(), 65535, 0, 1000, errbuf); //This line opens the ntwork interface for packet capture.
	if (!raw) { 
		/* report errbuf(makes sure pcap_open_live() worked)*/ 
		std::cout << "pcap_open_live failed: " << errbuf << std::endl;
		return 1;
	}
	
	using PcapHandle = std::unique_ptr<pcap_t, decltype(&pcap_close)>; //This will use pcap_close() instead of delete
	PcapHandle handle(raw, &pcap_close);
	
	if (pcap_datalink(handle.get()) != DLT_EN10MB) { //DLT_EN10MB just means Ethernet.
		// this milestone supports Ethernet capture only, so if the interface like isn't ethernet do:
		std::cout << "Only Ethernet capture is allowed" << endl;
	}
	else {
		//This code block is to read the packes with pcap_next_ex()
		pcap_pkthdr* header = nullptr; //Buffer check
		const u_char* bytes = nullptr; //Buffer check
		int rc = pcap_next_ex(handle.get(), &header, &bytes); //Actually reading the packets
		if (rc == 1) {
			auto packet = parser.parse(bytes, header->caplen, header->len); 
			//Take the packet, give it to parser(auto data type lets C++ figure out data type)
			//tell it how many bytes were captured, how long OG packet was,
			//and store the parsed result in packet.(caplen stand for capture length)
			if (packet.has_value()) {
				if (packet->protocol == TransportProtocol::TCP) {
					std::cout << "TCP ";
				}
				else if (packet->protocol == TransportProtocol::UDP) {
					std::cout << "UDP ";
				}
				std::cout << packet->source_ip << ":" << packet->source_port
					<< " -> " << packet->destination_ip << ":" << packet->destination_port
					<< " " << packet->wire_bytes << " B" << std::endl;
			}

		}
		else if (rc == 0) {
			std::cout << "timeout" << endl;
			//timeout; continue
			//YOU MAY NEED SOME ERROR HANDLING HERE IN CASE OF A TIME OUT
		}
		else if (rc == -1) {
			std::cout << "Capture error" << endl;
		}
	}
	
	return 0;
}


