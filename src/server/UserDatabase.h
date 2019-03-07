#pragma once
#include <string>
#include <vector>
#include <chrono>
#include <iostream>

#include "ClientInformation.h"
#include <raknet/BitStream.h>

using namespace std;

class UserDatabase
{
public:
	UserDatabase();
	~UserDatabase();
	void ConnectUser(const RakNet::Packet* Packet);
	void RemoveUser(const RakNet::Packet* Packet);
	string RegisterGuid(const RakNet::Packet* Packet);
	string FindUsername(string guid);
	string FindGuid(string username);
	vector<string> GetAllUsers();

private:
	vector<ClientData*> Users;
};

