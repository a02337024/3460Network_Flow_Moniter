### Requirements Analysis ###
Goal 1: Identify the major functional requirements of the system as is
Goal 2: Redesign the system using appropriate classes and their relationships
Goal 3: Plan how to test the refactored System

=== Requiremnts Analysis ===
R1: System must take an input from the user in the form of:
MSYS_NO_PATHCONV=1 ./main.exe '{network}' #sort_by_type #lines #size per report
where the #sort_by_type is inputted as 'packets' or 'bytes'
and the two: #lines #size per report are inputted as ints

R2: System must only capture ethernet interfaces

R3: System must be able to handle a user canceling the program through ctrl+C
in a clean manner.  

R4:	When the system is given incorrect input i.e. not enough arguments, 
incorrect argument type, or a bad port, the program must give a clean
understandable error through error handling. 

R5: Program must give a report based on the time interval specified, but
cap the number of captures by the specified #line amount given by the user
sorted by the metric of either packets or bytes.

=== System Design ===
Overarching main class: PacketProcessor
The PacketProcessor class will also have two subclasses that inherit from the
PacketProcessor class. Those two classes will be: PacketParser and PacketInfo

PacketParser subclass:
The PacketParser will be resposible for parsing out the information in the 
packet. It's specific responsiblilites will be:

1. verifies that the packet is within the bounds, and that the packet is 
either a UDP or TCP type packet.
2. obtain the source_ip and destination_ip addresses. 
3. Combines the low and high bytes. 
4. Identifies if the packet is UDP or TCP, and correctly attributes that
into the info object

PacketInfo struct(part of PacketProcessor):
The PacketInfo will be responsible for setting up the structures that hold
the attributes of the packets
1. The PacketInfo class will be responsible for creating the flowkey struct
that can take the source's and destination's ip address and port. 
2. Second it will be responsible for determining if the next packet is a new
packet with new information or not. 

FlowAggregator subclass:
The flow aggregator will be responsible for tracking and reporting packet 
data. The flow aggregator will also be responsible for updating the 
reporting, and keep track fo the time between flows. 
1. define the stats object to keep track the number of packts. 
2. define the compareByBytes method that will define the current and 
previous packets. 
3. define the compareByPacket method to defien the current and previous
packet by packet size. 
4. Define the sort and limit method for sorting the packets either by
packet size or bytes. 
5. Reporter:
	a. defines the printTitle method for printing the report title.
	b. defines the report method for reporting each pieve of traffic. 

Struct PacketInfo(part of the flowAggregator):
1. Defines the source_ip, source_port, destination_ip, and destination_port
2. Defines whether the protocl, sourc ip, source port, destination ip,
and destination port will be returned.
3. define a flowaggregator class to define update method, and flowTable.
4. Define the flowAggregator::update function:
	a. First pass the packet info to the key
	b. Compare the byte count for each packet
	c. Compare by packet key. 
	d. Check to see which sorting method was choosen 
	e. Call CcompareByBYtes or comparByPackets depending on user input.


Struct FlowKey:
1. Defines the source_ip, source_port, destination_ip, destination_port
and the TransportProtocol object. 
2. defines the operator function
	a. compares protocol
	b. compares source_ip
	c. compares source_port
	d. compares destination_ip
	e. compares desitination_port

Tests:
R1: System must take an input from the user in the form of:
MSYS_NO_PATHCONV=1 ./main.exe '{network}' #sort_by_type #lines #size per report
where the #sort_by_type is inputted as 'packets' or 'bytes'
and the two: #lines #size per report are inputted as ints

Test: Test different inputs such as an incorrect ip address, incorrect
input for sort type, and incorrect amount for lines printed.

Expected output: an error message stating: "incorrect _____"
where _____ is the incorrect input

R2: System must only capture ethernet interfaces

Test: get a premade program to see what the other interface traffic is. 
See if the system captures just ethernet

Expected output: Only Ethernet traffic that will be comapred against a
known source for verification

R3: System must be able to handle a user canceling the program through ctrl+C
in a clean manner. 

Test: Run the program at different phases, and cancel through ctrl-C, and
make sure that the exit is clean leading to no errors. 

Expected output: either a message saying: "program canceled" or a blank
clean screen with no errors.

R4: When the system is given incorrect input i.e. not enough arguments, 
incorrect argument type, or a bad port, the program must give a clean
understandable error through error handling. 

Test: Run the program with different port inputs, style inputs, sort
types, lines, and make sure there is proper error handling.

Expected output: an error message statign the specific portion of the
input was incorrect, and what the correct input should be. 

R5: Program must give a report based on the time interval specified, but
cap the number of captures by the specified #line amount given by the user
sorted by the metric of either packets or bytes.

Test: Test the program on different systems, and with different inputs
with different ports to make sure the reporting it consistent.

Expected output: similar output as to what is given when run the base
system that it was designed on. 


-Requirement 1 will test different input types, and make sure that proper
error handling is done, so that the user knows what they inputted wrong,
and how they can fix the program.
-Requirement 2 will ensure that only ethernet traffic is taken into the
program, as that is all it was designed for, so that the user will have
the expected outcome occour when using the device.
-Requirement 3 will make sure that the user is able to cancel the program
in a clean manner as is usual in other programs.
-Requirement 4 is used to ensure that the correct number and types of 
arguments are given, and the user is instructed as to what is wrong with
the input, and how it could be changed so that the program will run to
hopefully increase user satysfaction.
-Requirement 5 is put in place so that when the program is run on other
machines, then it is ejsured that the user experience will be the same
for all users. 

Bonus: The problem currently with the code is that there are some if-else
statements in the comparison strategy of sort_and_limit for the 
compareByBytes and compareByPackets functions. A factory pattern would
be more ideal for organization and implementation of the code. The 
implementation will help with cleaning up the code for the if-else
sorting. It will also help organize the code, and focus the code more
on the implementation. Lastly, with the poor error handling that has so
far been implemented, the factory method will allow for the creation of
more reusable methods for error handling that can be used across the 
program, to lead to better error handling and messages for user 
experience. 


Additionally, possibly creating a Reporter factory might be a good option
for other outputs for the different types(bytes or packets, UDP or TUP). 

The previously designed PacketInfo and FlowKey structs will be maintained
since they will not be needed to implement different behaviours that will
change from input to input. 

The expected improvement is a more organized code, and possibly more 
oppertunities for inline functions to save code space when implementing
the different methods. 


