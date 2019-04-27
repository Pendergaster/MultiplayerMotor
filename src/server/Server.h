#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <WinSock2.h>
#include <Windows.h>
#include <sstream>
#include <chrono>
#include <thread>

#include "MessageCodes.h"
#include "UserDatabase.h"
#include "UserVariables.h"

#include "UserDatabase.cpp"

/*Including basic raknet headeres*/
#include <raknet/RakPeerInterface.h>
#include <raknet/MessageIdentifiers.h>
#include <raknet/RakNetTypes.h>
#include <raknet/BitStream.h>

#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>

#define CONSOLE(x) std::cout << x << std::endl;

using namespace std;

class Server
{
	/*Public Functions*/
public:
	Server(string IP, int Port);
	~Server();
	void ServerStart();
	void InitBulletWorld();
	void ServerStop();
	void ServerUpdate();
	void CheckPacket(const RakNet::Packet& P);
	bool AskForVariable(CustomMessages var, INT64 guid);
	bool AskForVariable(CustomMessages var, string username);
	void RequestFromAll(CustomMessages var);
	void SendResponse(RakNet::SystemAddress sys, CustomMessages responseID);
	void SendSlotID(RakNet::SystemAddress addr, int id);
	void BroadcastVar(CustomMessages Var, RakNet::Packet Packet);
	void SendCubeInfo();
	void ReadBulk(RakNet::Packet* packet);
	void WriteBulk();
	void AddPlayerCube(std::string name);
	void RemovePlayerCube(std::string name);

	void ReadPlayerInput(RakNet::Packet* packet);
	void ReadPlayerCoord(RakNet::Packet* packet);
	/*Private variables*/
private:
	RakNet::Packet* Packet;
	RakNet::RakPeerInterface* Peer;
	RakNet::SocketDescriptor* SD;
	RakNet::BitStream bsIN;
	RakNet::BitStream bsOUT;

	UserDatabase* Connections;

	string IP;
	int Port;
	int MaxConnections = 10;

	bool State = true;
	bool running = false;
	string Result;

	/*Tickrates*/
	chrono::time_point<chrono::system_clock> Delta120;
	float TimeInterval;

	std::vector<btRigidBody*> cubes;
	btRigidBody* planerb;
	std::vector<btRigidBody*> players;
	std::vector<std::string> slots;

	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher * dispatcher;
	btBroadphaseInterface * overlappingPairCache;
	btSequentialImpulseConstraintSolver * solver;
	btDiscreteDynamicsWorld * dynamicsWorld;

};
