#pragma once
#include <raknet/RakNetTypes.h>

#include <iostream>
#include <string>
#include <chrono>


using namespace std;

struct ClientData
{
	ClientData(chrono::system_clock::time_point ConnectionTimestamp, string ConnectionID,RakNet::SystemAddress address) {
		this->ConnectionTimestamp = ConnectionTimestamp;
		this->ConnectionID = ConnectionID;
		this->Username = "NONE";
		this->Address = address;
	}
	RakNet::SystemAddress Address;
	chrono::system_clock::time_point ConnectionTimestamp;
	string ConnectionID;
	string Username;
};