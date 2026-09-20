#include <iostream>
#include <stdexcept>
#include <pcap.h>
#include <memory>
using namespace std;

int main(int argc, char*argv[1]) {
	if (argc > 2) {
		throw runtime_error("Cannot accept more than 1 argument");
	}
	if (argc < 2) {
		std::cerr << "Error: MUst provide a network interface." << std::endl;
		return 1;
	}
	PacketParser parser; //defines the PacketParser object
	std::string interface = argv[1]; //gets the desired interface from the user
	char errbuf[PCAP_ERRBUF_SIZE]{}; //This is an empty character array where array messages will be stored
	//(PCAP_ERRBUF_SIZE is from libpvap and specifies how large errbuf should be)
	pcap_t* raw = pcap_open_live(interface.c_str(), 65535, 0, 1000, errbuf); //This line opens the ntwork interface for packet capture.
	if (!raw) { 
		/* report errbuf(makes sure pcap_open_live() worked)*/ 
		std::cout << "pcap_open_live failed" << endl;
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
			auto packet = parser.parse(bytes, header->caplen, header->len); //WHERE IS THE PARSER OBJECT DEFINED!!!!
			//Take the packet, give it to parser(auto data type lets C++ figure out data type)
			//tell it how many bytes were captured, how long OG packet was,
			//and store the parsed result in packet.(caplen stand for capture length)
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


