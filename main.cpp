#include <iostream>
#include <stdexcept>
#include <pcap.h>
#include <memory>
#include "packet_parser.h"
#include "packet_Info.h"
#include <chrono>
#include <thread>
using namespace std;
using Clock = std::chrono::steady_clock;

int main(int argc, char*argv[]) {
	//argv[0] = name of the program
	//argv[1] = interface provided by the user(see the defintion of input
	//argv[2] = sort_by (bytes/packets)
	std::string sort_by = argv[2];
	long Interval = std::stoul(argv[3]);
	std::size_t top_n = std::stoul(argv[4]);
	
	//Will take "bytes" and "packets" as an argument. 
	if (argc > 5) {
		throw runtime_error("Cannot accept more than 1 argument");
		return 1;
	}
	if (argc < 3) {
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
	pcap_t* raw = pcap_open_live(interface.c_str(), 65535, 0, 1000, errbuf); //This line opens the network interface for packet capture.
	using PcapHandle = std::unique_ptr<pcap_t, decltype(&pcap_close)>; //This will use pcap_close() instead of delete
	PcapHandle handle(raw, &pcap_close);
	Reporter report;
	if (!raw) { 
		/* report errbuf(makes sure pcap_open_live() worked)*/ 
		std::cout << "pcap_open_live failed: " << errbuf << std::endl;
		return 1;
	}
	
	else if (pcap_setnonblock(handle.get(), 1, errbuf) != 0) {
		std::cout << "In the if pcap handle if statement";
		throw std::runtime_error(errbuf);
		//This will help the system not throw errors when there is not packet available. 
	}
	
	else if (pcap_datalink(handle.get()) != DLT_EN10MB) { //DLT_EN10MB just means Ethernet.
		// this milestone supports Ethernet capture only, so if the interface like isn't ethernet do:
		std::cout << "Only Ethernet capture is allowed" << endl;
		return 1;
	}
	else {
		pcap_pkthdr* header = nullptr; //Buffer check
		const u_char* bytes = nullptr; //Buffer check
		int count = 0;
		FlowAggregator aggregator;
		Clock::time_point last_report = Clock::now();
		//We want to make sure that it this is executed onece before the loop starts
		//The aggregator is what collects and adds the items to be displayed. 
		
			while (true) {
				//This code block is to read the packes with pcap_next_ex()
				int rc = pcap_next_ex(handle.get(), &header, &bytes); //Actually reading the packets
				if (rc == 1) {
					auto packet = parser.parse(bytes, header->caplen, header->len);
					//Take the packet, give it to parser(auto data type lets C++ figure out data type)
					//tell it how many bytes were captured, how long OG packet was,
					//and store the parsed result in packet.(caplen stand for capture length)
					if (packet.has_value()) {
						aggregator.update(*packet); //Calls update for any valid packet returned by the parser. Will retun packet
						//update() comes from the Flow aggregator class in packet_Infor.h

						count += 1;
						
					}

				}
				else if (rc == -1) {
					std::cout << "Capture error" << endl;
				}
				Clock::time_point now = Clock::now();
				if (now - last_report >= (std::chrono::seconds(Interval))) {
					//This part is for reporting the flows.
					FlowSnapshot rows = aggregator.snapshot();
					const std::size_t total_flows = rows.size();

					last_report = now;
					sort_and_limit(rows, sort_by, top_n);
					report.printTitle(Interval, sort_by, aggregator.snapshot().size(), total_flows);
					//Prints the title for the report.
					report.print(rows);
				}
				if (rc == 0) {
					std::this_thread::sleep_for(std::chrono::milliseconds(10));
					//Lets the computer sleep so that it can then get the next packet. 
				}
				//destructor to make sure the FlowAggregator and packet is destroyed each time.
			}
	}
	return 0;
}


