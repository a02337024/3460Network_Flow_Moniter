#pragma once
#include <string>
#include <cstdint>
#include <map>
#include <vector>
#include <iostream>
#include <algorithm> //This is needed to use std::sort
using namespace std;

enum class TransportProtocol { TCP, UDP };
//enum class is only for defining constants.
struct PacketInfo {
	std::string source_ip;
	std::string destination_ip;
	std::uint16_t source_port{};
	std::uint16_t destination_port{};
	TransportProtocol protocol{};
	std::uint32_t wire_bytes{};
};
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
};

struct FlowStats {
	std::uint64_t packets{};
	std::uint64_t bytes{};
};

struct FlowRow { FlowKey key; FlowStats stats; };
using FlowTable = std::map<FlowKey, FlowStats>; //The original had _In_place_key_extract_map, but that gave errors 
//The internet said to use std::map instead.
using FlowSnapshot = std::vector<FlowRow>; //Reporting copy

class FlowAggregator {
public:
	void update(const PacketInfo& packet);
	FlowSnapshot snapshot() const;
private:
	FlowTable flows_; //Persists across reports
};

inline void FlowAggregator::update(const PacketInfo& packet) {
	FlowKey key{ packet.protocol, packet.source_ip, packet.source_port, packet.destination_ip, packet.destination_port };

	FlowStats& stats = flows_[key];
	++stats.packets;
	stats.bytes += packet.wire_bytes;
}
	//This section will sort, then it will display the copy.
	inline bool compareByBytes(const FlowRow& a, const FlowRow& b) {
		if (a.stats.bytes != b.stats.bytes){
			return a.stats.bytes > b.stats.bytes;
		}
			return a.key < b.key;
	}

	inline bool compareByPackets(const FlowRow& a, const FlowRow& b) {
		if (a.stats.packets != b.stats.packets) {
			return a.stats.packets > b.stats.packets;
		}
		return a.key < b.key;
	}
	inline void sort_and_limit(FlowSnapshot& rows, const std::string& sort_by, std::size_t top_n) {
		if (sort_by == "bytes") {
			std::sort(rows.begin(), rows.end(), compareByBytes);
		}
		else {
			std::sort(rows.begin(), rows.end(), compareByPackets);
		}
		if (rows.size() > top_n) {
			rows.resize(top_n);
		}
	}


inline FlowSnapshot FlowAggregator::snapshot() const {
	//Makes a reporting copy
	FlowSnapshot rows;
	rows.reserve(flows_.size());
	for (const std::pair<const FlowKey, FlowStats>& entry : flows_) {
		rows.push_back(FlowRow{ entry.first, entry.second });
	}
	return rows;
}

class Reporter {
public:
	void printTitle(auto interval_Seconds, std::string sort_by, int size, int totalFlows) {
		std::cout << "Interval: " << interval_Seconds << " s | Sort: " << sort_by << " | Flows stored : " << size << " | Showing : " << totalFlows << std::endl;
		std::cout << "PROTO\tSOURCE\t\t\tDESTINATION\t\tPACKETS\tBYTES" << std::endl;
	}
	void print(FlowSnapshot rows) {
		for (const auto& row : rows) {
			if (row.key.protocol == TransportProtocol::TCP) {
				std::cout << "TCP ";
			}
			else if (row.key.protocol == TransportProtocol::UDP) {
				std::cout << "UDP ";
			}
			std::cout << row.key.source_ip << ":" << row.key.source_port
				<< "\t\t" << row.key.destination_ip << ":" << row.key.destination_port << "\t\t" << row.stats.packets
				<< "\t" << row.stats.bytes << std::endl;
		}
	}
	

};
