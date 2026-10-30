### Notes for Milestone Two ###
Main.cpp will be responsiblilbe for getting the arguments from the user,
namely the desired network, the sort type(bytes or packets), size, and 
number of lines per report. 


=== outline for main.cpp ===

Note, in the future, this function should be split up into different functions 
   for better readability
1. Take user input, check input, and use pcap_next_ex to open the packet
	a. Making sure the currect number of arguments is inputted
	b. Prints what interface is being opened
	c. Opens the packet using pcap_open_live
	d. Ensures that the proper capture type is inputted

2. creation of aggregator object, and packet parsing
	a. Creates FlowAggregator object, and Clock object
	b. In while loop
		i. Checks to make sure that packet was obtained
		ii. Calls the packet parer to parse the packet
			(See outline packet_parser.cpp for notes on this)
		iii. Increases a count for the number of packets captured. 
		iv. Obtains the total size of packet
		v. Gets the total time it took
		vi. Prints the report using the printTitle method. 
		vii. Lets the computer sleep to prepare for next packet capture. 
	c. Calls return 0 to ensure that 

		
=== outline for packet_parser.h ===

1. outples the class PacketParser with the prototype for the PacketInfo prototype


=== outline for packet_parser.cpp ===\

Note, in the future, this function should be split up into different functions 
   for better readability 
1. Creates function for packet parsing. 
	a. Ensure that the packet length is short enough for processing.
	b. Define variables for obtaining the bytes of the ethernet and the ip address
	c. Ensures that the protocol number is the correct length
	d. Defines the source and destination IP address variables
	e. Formats the source_ip and destination_ip address variables
	f. Ensures that protocl num is the correct lenght
	g. Combines all infor into for the sourc eand destination port
	h. Defines the PacketInfo object, and inputs all information into the object
	   attributes. 
	i. Returns the object
	

=== outline for packet_Info.h ===

1. Defines TransportProtocol class

2. Struct for PaketInfo
	a. Defines PacketInfor members: source_ip, destination_ip, source_port{}, 
	   destination_port{}, a TransportProtocol object, and wire_bytes_
	
3. Struct for FlowKey
	a. a TransportProtocol object, source_ip, sourceport{}, destination_ip
	   and destination_port variable. 
	b. defines the operator method that compres the: protocol, source_ip, 
	   source_port, destination_ip, and destination port
	   part be is done to check to see if a packet is new, or the same packet
	   so that each updated packet is a new packet. 
	
4. Struct for FlowStats
	a. For the flow key, defines the packet and bytes members
	
5. Struct for FlowRow
	a. Used for the flow key, and FlowStats
	
6. FlowAggregator class
	a. Defines the flowsnapshot object and flowtable.

7. FlowAggregator::update method 
	a. Defines a FlowKey object
	b. Defines a flow key stats, and updates the stats object for counting packets
	c. Creates a compareByBytes method for sorting the packets by bytes
	d. Creates a compareByPackets method for sorting the packets by packet stats
	e. Implementation of the compareByBytes and compareByPackets methods

8. FlowAggregator::snapshot method
	a. Defines a FlowSnapshot object
	b. Reserves space in the rows object big enough to take flows_.size()
	c. Puts the FlowRow struct(point 5.)
	d. Returns the rows object

9. Reporter class
	a. Method defined for printing title for the flow report
	b. Method defined for printing each packet captured

