### This is a file completely for notes and planning on the Network Flow Moniter project ###
For live packet capture, libpcap will be used.
From the web, information regarding libpcap was found at: tcpdump.org //libpcap was downladed
from this website as well

Workflow:
/Start
1. Selected interface
  
2. PcapCapture (libpcap)
     packet bytes + caplen
     -The responsibility of the PCapCapture is to:
        1. Open the interface
        2. own pcap_t
        3. retrieve packets

3. packet Parser
   -The responsibility of the packetParesr is to:
     1. Ethernet
          2. IPv4
               3. TCP/UDP(With bounds check)
          PacketInfo
               Reporter
               -The responsibility of the packetInfo/Reporter is to:
                  1. Own parsed metadata and
                  2. print one concise line

Note: Consider usering threads to communicate througha  shared in-memory flow
table. For example, Have the capture function on one thread, and the reporter
be on another thread. Take a look at the Concurrency slides for more info.
/End


    
