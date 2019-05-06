#pragma once

#include "MessageCodes.h"
#include "inputs.h"
#include "objecttype.h"
/*Including basic raknet headeres*/
#include <raknet/RakPeerInterface.h>
#include <raknet/MessageIdentifiers.h>
#include <raknet/RakNetTypes.h>
#include <raknet/BitStream.h>

#include <iostream>
#include <string>
#include <WinSock2.h>
#include <Windows.h>
#include <sstream>
#include <thread>
#include <vector>
#include <chrono>

#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
//#include "Utility.h"


#define CONSOLE(x) std::cout << x << std::endl;

using namespace std;

class Client
{
/*PUBLIC FUNCTIONS*/
public:
	Client(string IP, int Port,const char* username);
	Client() {};
	//Client(){}
	~Client();
	void Init(std::string IP, int Port, const char* username);
	CustomMessages Update();
	void OpenConnection();
	void CloseConnection();
	void RetryConnection();
	void SendUsernameForServer(RakNet::RakString username);
	RakNet::RakString GetUsername() { return RakNet::RakString(m_username.c_str());}
	void UsernameChange(std::string* username);
	void ReadPlayerSlot(RakNet::Packet* packet);
	void CheckForVar(CustomMessages messageID);
	void SetVar(CustomMessages MessageID, std::vector<string*> Vars);
	void SetVar(CustomMessages MessageID, std::vector<float*>Vars);
	void SetVar(CustomMessages MessageID, std::vector<int*>Vars);
	void ReadBulk(RakNet::Packet* packet);
	void ReadCubeInfo(RakNet::BitStream* bs);
	void ReadPlayerInfo(RakNet::Packet* packet);
	void SendPlayerState();

/*PRIVATE FUNCTIONS*/
private:
	CustomMessages ClientConnectionUpdate(RakNet::Packet* Packet);

/*PUBLIC VARIABLES*/
public:
	std::string m_ip;
	int m_port;
	std::string m_username;

	bool Connected = false;
	bool LoggedIn = false;
	bool State = true;
	vector<Var<int>>IntVars;
	vector<Var<string>> StringVars;
	vector<Var<float>> FloatVars;
	vector<MessageType> registeredServerValues;
	std::thread BackupThread;

	std::vector<ObjectTracker> Objects; //Paten objectit
	std::vector<PlayerData> Players;

	int previousPacketID = 0;
	int packetID = 0;
	bool isNewData = false; //TÄMÄ KERTOO ONKO UUTTA DATAA TULLUT.

	std::vector<btVector3> playerPos;
	std::vector<btQuaternion> playerRot;
	int playerSlot;

	vec3 lookDir;
	inputType input;

/*PRIVATE VARIABLES*/
private:

	RakNet::SystemAddress HostAddress;
	RakNet::RakPeerInterface* Peer;
	RakNet::Packet* m_packet;
	RakNet::SocketDescriptor SD;
		
	std::chrono::system_clock::time_point Delta;
	float TimeInterval;
};

