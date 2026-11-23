### Requirements Analysis ###
Goal 1: Identify the major functional requirements of the system as is
Goal 2: Redesign the system using appropriate classes and their relationships
Goal 3: Plan how to test the refactored System

=== Requirements Analysis ===
R1(Argument Parsing): The program shall accept a target network interface, a
sort metric of either packets or bytes, a maximum report rows, and report
interval. The program shall exit the program with the appropriate error 
message if given missing, malformed, or out-range arguments. This shall
include strings for integer based arguments, and invalid port numbers

R2(Ethernet capture only): After packet capture, the program will filter the
packets so that only ethernet based packets are processed.

R3(Safe parsing): Before packets are parsed the packet length shall be 
validated before reading fields and extracting packet attributes. The
program then must safely discard truncated frames or unsupported networks and
transport protocols without reading out of bound packet pieces or crashing.

R4(grouping Directional five-tuple packet flows): The flow aggregator shall
collect src_ip, src_prt, dst_ip, dst_port, protocol(UDP or TCP) data from
each packet treating a flow from one source to another source as distinct
from that same destination to source i.e. a flow from A to B is treated 
differently than a flow from B to A. 

R5(Periodic reporting): The program shall produce a formatted report summarizing
the network flow traffic ordering the flow report based on the metric choosen
by the user(packets or bytes). This flow should include when zero matching
packets arrive during the interval time. 

R6(Graceful SIGINT(Interrupt signal) handling): When the user executes the 
Ctrl-C command(SIGINT), the program shall stop network capture and reporting
cleanly(giving no errors), but will emit the current report that was being 
compiled before the stop. It will exit with status 0 with no extra output 
beyond the current report.

=== System Design ===
Class 1. Argument Parser:
Responsibility: Validates interface, sort metric, max lines, and interval time.
before capturing starts, returning the correct configuration or a clear error
message. This class will fulfill R1 through taking user input and fulfill R6
through the error handling.

Key Data Members:
std::string sort_by
long Interval
std::size_t top_n

Interactions with other classes:
The argument parser will take the user input of the target network interface,
the sort metric, the maximum report rows, and the report intervals. the 
argument parser will then take the Interval object and pass it to the 
FFlowMoniter
vairables: sort_by, Interval, and top_n

Class 2. FlowMonitor
Responsibility: Executes the capture and report loop. In that loop it will 
execute the packetCapture. This class will help fulfil R1-R5 through taking
the user input specified in R1, Ensure that only Ethernet traffic is captured
specified by R2(PacketCapture), validates packet length to satisfy R3
(PacketParser), Groups the flows R4(Reporter), Periodically produces a report
through the reporter class in the manner specified by R5(Reporter), Will 
handle SIGINT in the main FlowMoniter class to fulfil R6.

Data members:
Variables: std::chrono::millisenonds interval, size_t lines
Objects: PacketCapture capture, PacketPaser parser, FlowAggregator aggregator,
Reporter reporter

Interactions with other classes:
The FlowMoniter shall use PacketCapture, PacketParser, the FlowAggregator, and
Reporter. This will be done to initiate the capture(by opening captured packets) 
in PacketCapture, aggregation(combining the acquired data), and reporting of 
the network flows through the reporter class. By passing the PacketInfo values 
to the aggregator the aggregator will be able to sort the packets, and pass
the information to the reporter, which when called by the FlowMoniter, will 
print the necessary report.

Class 3. PacketCapture
Responsiblities: Utilizes the libpcap library to take raw packets using the 
pcap_open_live function. The PacketCapture will handle runtime errors(when
there is not a packet available), an error when pcap_open_live failes, and
when a network that is not ethernet is given(ignores the traffic)R2.

Data Members: 
Variables: std::string interface, errbuf, 
Objects: PcapHandle handle(note this is pointer object)
pcap_t raw
Reporter report

Interactions with other classes:
The FlowMoniter will control when PacketCapture is opened, the packet length, 
and when the program breaks out of PacketCapture(and when the handler is 
closed). After the packet is opened the contents will be fed to the 
packetParser

Class 4. PacketParser
Responsibilites: The Packet parser will first validate the packet length, 
truncate and discard any parts of the packet that are too long, and ensure
that is of the correct type to fulfil R3. The parserthen uses the PacketInfo 
struct to input the extracted packet information to later be used by other 
parts of the program.

Data Members:
FlowKey: uint32_t src_ip, uint32_t dst_ip, uint16_t src_port, uint16_t, 
uint_8 protocol
PacketInfo: FlowKey key, uint64_t wire_bytes, uint64_t timestamp_us

Interactions with other classes:
The PacketCapture will give the raw packet to the PacketParser after it has 
been verified to be a valid packet, so that it wraps packet inside of the
PacketInfo struct. It will do this by constructing a FLowKey inside of
a PacketInfo struct. The parts included in this struct are the source ip,
source port, destination ip, destination port, adn the protocol. This
is done so that the flowAggregator recieves formatted information, and 
never recieves invalide data. Fulfils R3. The reporter also utilizes the
FlowKey to access the source and destination ips and ports. 

Class 5. FlowAggregator
Responsibility: The Flow aggregator shall maintain a record of all the 
flow statistics including the two addresses and the flow direction data 
collected during the time period specified by the user to fulfil R5 and R4. 
The flow aggreagator shall also rank the rows using the sort stategy the 
user to fulfil R1.

data members:
FLowKey key
FlowStats& stats
FlowStats: uint64_t packets = 0, uint64_t bytes = 0
Methods:
FlowAggregator::snapshot()
SortStrategyFactory(for either sort by bytes or sort by packets)

Interactions with other classes:
The flowAggregator will use two functions: either comparByBytes or 
compareByPacket, and it will use the sort_and_limit funcitons to sort the
different packets based on the user input.R1 and R4.Secondly, the flow
aggreagator will use a FlowSnapshot to give the rows of data to the 
Reporter class assisting in fulfilling R4 and R5. 

Class 6. Reporter
Responsibility: The reporter shall format and print the interval shapshots. 
The Reporter will print for the user, the interval time, the sort type,
flows stored, and total flows as a title. under the title the program shall
print the flow stats for each packet including: wether it is UDP or TCP, 
The source_ip, source_port, destination_ip, destination_port, and either
the packets, or number of bytes R5 and R6 depending on the user input R1.

Data Members:
auto row
row.key
row.stats
row utilizes the key struct

Interactions with other classes:
The Reporter(via the FlowMoniter) recievers data form the FlowAggregator,
which it hten formats for the report. 

Struct 1. FlowKey
Responsibility: represent the data 5-tuple identifier members:
uint32_t src_ip, uint32_t dst_ip, uint16_t src_port, uint16_t dst_port,
uint8_t protocol

Code declaration of FlowKey:
struct FlowKey {
	TransportProtocol protocol{};
	std::string source_ip;
	std::uint16_t source_port{};
	std::string destination_ip;
	std::uint16_t destination_port{};

	bool operator<(const FlowKey& other) const {
		if (protocol != other.protocol) {
			return protocol < other.protocol;
		}
		if (source_ip != other.source_ip) {
			return source_ip < other.source_ip;
		}
		if (source_port != other.source_port) {
			return source_port < other.source_port;
		}
		if (destination_ip != other.destination_ip) {
			return destination_ip < other.destination_ip;
		}
		return destination_port < other.destination_port;
	};

Struct 2. PacketInfo
Responsiblitiy: represents the FlowKey key, uint64_t wire_bytes, and
the uint64_t timestamp_us.

Code declaration of PacketInfo:
struct PacketInfo {
	std::string source_ip;
	std::string destination_ip;
	std::uint16_t source_port{};
	std::uint16_t destination_port{};
	TransportProtocol protocol{};
	std::uint32_t wire_bytes{};
};

Struct 3. FlowStats
Responsibility: Reperesent the uint64_t packets, uint64_t bytes
(Both start initialized as 0)

Code declaration of FlowStats:
struct FlowStats {
	std::uint64_t packets{};
	std::uint64_t bytes{};
};

Note that the structs help fulfil R4 and R5, since the structs help with
organizing the data for reporting.SInce the structs are also used for 
holding the data after it is parsed, the structs also help with safe 
parsing of the data helping to fulfil R3

Diagram:
+---------------------------------------------------------------------------------------------------+
|Flow Moniter																						|
+---------------------------------------------------------------------------------------------------+
|-interval		: chrono::milliseconds																|
|-lines			: size_t																			|
|-capture		: PacketCapture																		| 
|-parser		: PacketParser																		|
|-aggregator	: FlowAggregator																	|
|-reporter		: Reporter																			|
+---------------------------------------------------------------------------------------------------+
	|							|						^									|
	V							V						V									V
+---------------------+		+---------------+		+---------------------------+		+---------+
|PacketCapture        |		|PacketParser   |		|FlowAggregator				|   ->	|Reporter |
+---------------------+		+---------------+		+---------------------------+		+---------+
|PcapHandle handle    |		|PacketInfo	    |		|FlowKey key				|		|auto row |
|pcap_t raw	          |		|FlowKey key    |		|FlowStats stats			|		|row.key  |
|Reporter report      |		+---------------+		|FlowAggregator::snapshot()	|		|row.stats|
|std::string interface|			|					|SortStrategyFactory		|		+---------+
|errbuf				  |			|					+---------------------------+			^
+---------------------+			|								|							|
								|								V							|
								|  +-------------+	+-------------------+  +-----------+    |
								|  |SortByPackets|<-|SortStrategyFactory|->|SortByBytes|	|
								|  +-------------+  +-------------------+  +-----------+	|
								V															|
							+---------------------+		+-------------------+		+-------------------+
							|PacketInfo			  |---->|FlowKey			|------>|FlowStats			|
							+---------------------+		+-------------------+		+-------------------+
							|FlowKey key          |		|uint32_t src_ip	|		|uint64_t packets	|
							|uint64_t wire_bytes  |		|uint32_t dst_ip	|		|uint64_t bytes		|
							|uint64_t timestamp_us|		|uint16_t src_port	|		+-------------------+
							+---------------------+		|uint16_t dst_port	|
														|uint8_t  protocol	|
														+-------------------+
Tests
1. Valid argument input
Target: R1(Argument Parsing)
test1:
Input: sort metric: bytes, row limit: 5, interval: 1000
Expected output: Argument is parsed cleanly(no errors or other output before
the first report), and the configuration is sort_by == "bytes", top_n == 5, 
and interval == 1000 ms
test2:
input: sort metric: packets, row limit: 5, interval: 1000
Expected output: Argument is parsed cleanly(no errors or other output before
the first report), and the configuration is sort_by == "packets", top_n == 5, 
and interval == 1000 ms
test3:
Input: sort metric: alphabetical, row limit: 4, interval: 2000
Expected output: error is given "invalid input"
test4:
Input: sort metric: packets, row limit: 6, interval banana
Expected output: error is given "invalid input"

2. Non-Ethernet interface
Target: R2 (Ethernet Capture Only)
Input: Pass a non-Ethernet interface to the program
Expected output: An error message informing the user only ethernet capture is
available.
Input: synthetic frames with non-Ethernet link types.
Expected output: An error message informing the user only ethernet capture is
available.

3. Valid TCP and UDP Packet Parsing
Target: R3(Safe parsing)
Input: Valid Ethernet interface
Expected output: Ethernet TCP and Ethernet UDP with known src_ip, dst_ip, 
src_port, dst_port, protocol will be correctly printed in the report. 
Note, this will be checked with a known working network flow moniter program

4. Truncated Frames and unsupported protocols
Target: R3(Safe parsing)
test1:
Input: Supply a buffer that is shorter than the program needs
test2:
Input: Supply an IPv4 with an invalid IHL
test3:
Input: IPv4 packet carrying an unsupported transport protocol
Expected output:For all three tests the PacketParser should reject the 
packets cleanly and safely.

5. Direction Five-Tuple Grouping
Target: R4(grouping Directional five-tuple packet flows)
Input: Two TCP packets form two different sources in two different flow 
directions
Expected output: The program is expected to report two different flows,and
not to combine the two into one line on the flow report.

6. Periodic Reporting, Sorting, and Idle intervals
Target: R5(Perodic reporting)
test1:
Input: Generate a couple flows(at least 4), so that different packet and byte
counts are present. 
Expected output: the report will have a specific row for each flow, with the 
correct information on each packet.
test2:
Input: pause packet generation.
Expected output: during the idle interval, the program shall still gerenate a 
report that is empty(the title shall still print). 

7. Proper handling of SIGINT (Ctrl-C)
Target: R6(Graceful SIGINT(Interrupt signal) handling)
test1: 
Input: Start the program with any valid input, then execute SIGINT (Ctrl-C).
Expected output: The program should stop cleanly with no runtime errors, and the
current report being generated should still be emitted before the program fully 
stops. 
test2-5:
Input: For tests 2-5 the program should be given any valid input varying the 
different ip addressses, interval time, lines per report, and sort metric
running the program, and then executing SIGINT (Ctrl-C) 
Expected output: The program should stop cleanly with no runtime errors, and the
current report being generated should still be emitted before the program fully 
stops. 

Bonus Design Pattern:
The problem: The problem with the currect set up of the program(pre-refactoring)
is that there are a few different if else items that could be better organized
through a strategy for choosing different comparison rules. The prime example 
that could benefit from this is the selection of the sort metric when the 
program is handling sorting the flows by either packets or bytes. 

The pattern: The best pattern to use to solve this problem is strategy. Using 
strategy will allow the program to be able to choose which sorting method based
on if the user inputs packets or bytes, so that way the program will only have
to have one interface. 

Participating classes:
SortStrategyFactory

FlowAggregator:
1. SortByPackets:
Implements a method to sort the flows by packet count
2. SortByBytes:
Implements a method to sort the flows by byte count

FlowAggregator:
Since this the class that is in charge of sorting the flows, it will be the 
class to determine which strategy is implemented based on the user input.

Expected improvements:
Because the current method to aggregate and sort the flows is combined, it
makes it harder to debug, or add any other features in future updates to the 
system. If the the ranking and aggregation are decoupled it will make unit
testing cleaner(since each aggreagation and sorting can be tested seperately),
easier to add new sorting criteria if desired, or changing the way the flow 
information is combined for the report. 



