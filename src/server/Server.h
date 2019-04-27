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
#include "inputs.h"

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

struct Cube
{
	Cube(int id, int type, btRigidBody* rb) { this->id = id; this->Type = Type; this->rb = rb; };
	int id;
	int Type;
	btRigidBody* rb;
};

class Server
{
	/*Public Functions*/
public:
	Server(string IP, int Port);
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
	void ReadPlayerState(RakNet::Packet* packet);
	void WriteBulk();
	void AddPlayerCube(std::string name);
	void RemovePlayerCube(std::string name);
	void ReadPlayerLookDir(RakNet::Packet* packet);
	void UpdatePlayerCube(std::string guid,inputType input, vec3 lookDir);
	void ReadPlayerInput(RakNet::Packet* packet);
	void ReadPlayerCoord(RakNet::Packet* packet);

	/*Version 2*/
	void RemoveSmallCube(int id);
	void AddCube(int type, vec3 pos, vec3 rot);
	void SendSmallCubeInfo();

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

	/*Version 2 small cubes*/
	std::vector<Cube> smallCubesActive;
	std::vector<Cube> smallCubesInactive;

	std::vector<btRigidBody*> cubes;
	btRigidBody* floor;
	std::vector<btRigidBody*> players;
	std::vector<std::string> slots;

	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher * dispatcher;
	btBroadphaseInterface * overlappingPairCache;
	btSequentialImpulseConstraintSolver * solver;
	btDiscreteDynamicsWorld * dynamicsWorld;
	
	float MovementSpeedMultiplier = 50;
};
