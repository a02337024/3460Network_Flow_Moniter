HEAD
# 3460Network_Flow_Moniter
A repository for the work for the Network Flow Monitor project. 

Team Leader: Forrest Waters, A02337024, a02337024@aggies.usu.edu

To run main.cpp to get the printed result of "Hello, World!" 
first: compile by entering this command in the proper file path:
g++ main.cpp -o helloworld
second: run the program by entering this command:
./helloworld

# 3460 Network Flow Moniter
312579 (Initial commit)

# To run the program on windows:
Get the interface by running this command:
Get-NetAdapter | Select-Object Name, InterfaceGuid, Status
To get the network to pass the program.
Next: 

Run this command in the git terminal to compile the program:
g++ main.cpp packet_parser.cpp ./npcap-sdk-1.16/Lib/x64/wpcap.lib -o main.exe -I./libpcap1 -lws2_32

To run the compiled program in the git terminal do:
MSYS_NO_PATHCONV=1 ./main.exe '{Interface}' #bytes #lines per report
Where interface is the interfaceguid. 
Example:
MSYS_NO_PATHCONV=1 ./main.exe '{DFCFD80E-D1E6-4E09-B693-670A3FA79C95}' 10 10

To run the program in Ubuntu(for Linux) do:
To compile:
g++ main.cpp packet_parser.cpp -o main -lpcap
To run:
sudo ./main eth0


