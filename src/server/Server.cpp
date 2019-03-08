#include "Server.h"

using namespace RakNet;
using namespace std;

#define PORT 60000

Server::Server(string IP, int Port)
{
	string title = "Raknet-Server";
	SetConsoleTitle(title.c_str());
	this->IP = IP;
	this->Port = Port;
	Connections = new UserDatabase();
	ServerStart();

	InitBulletWorld();
}


Server::~Server()
{
}

void Server::InitBulletWorld()
{
	collisionConfiguration =	new btDefaultCollisionConfiguration();
	dispatcher =				new btCollisionDispatcher(collisionConfiguration);
	overlappingPairCache =		new btDbvtBroadphase();
	solver =					new  btSequentialImpulseConstraintSolver;
	dynamicsWorld =				new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

	dynamicsWorld->setGravity(btVector3(0, -4.f, 0));
	
	btTransform t;
	t.setIdentity();
	t.setOrigin({0,0,0});
	btStaticPlaneShape* plane = new btStaticPlaneShape(btVector3(0, 1, 0), 0);
	btMotionState* motion = new btDefaultMotionState(t);
	btRigidBody::btRigidBodyConstructionInfo info(0.0, motion, plane);
	planerb = new btRigidBody(info);

	btVector3 defaultIntertia(0, 0, 0);

	dynamicsWorld->addRigidBody(planerb);

	for (int x = 0; x < 3; x++) //tehdään cubet mappiin
	{
		for (int y = 0; y < 3; y++) //tehdään cubet mappiin
		{
			t.setOrigin({(float)x,1,(float)y});

			btBoxShape* box = new btBoxShape({ 1,1,1 });
			box->calculateLocalInertia(1.0f, defaultIntertia);
			btMotionState* boxmotion = new btDefaultMotionState(t);
			btRigidBody::btRigidBodyConstructionInfo boxInfo(1.0f, boxmotion, box, defaultIntertia);
			btRigidBody* newBox = new btRigidBody(boxInfo);
			newBox->setFriction(0.1f);
			cubes.push_back(newBox);
			dynamicsWorld->addRigidBody(newBox);
		}
	}
}

void Server::ServerStart()
{
	Peer = RakPeerInterface::GetInstance();

	SD = new SocketDescriptor(PORT, 0);
	/*Creating Instance for server and starting up basic setup*/
	Peer->Startup(MaxConnections, SD, 1);
	Peer->SetMaximumIncomingConnections(MaxConnections);
	CONSOLE("Starting server at port " << Port);

	Delta120 = chrono::system_clock::now();
	TimeInterval = (int)((1.0 / 60) * 1000);

	running = true;
}

void Server::ServerStop()
{
	running = false;
	RakPeerInterface::DestroyInstance(Peer);
	delete SD;
	delete Connections;
}

void Server::ServerUpdate()
{
	auto Delta = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - Delta120);
	/*Loads packet from peer*/
	if ((float)Delta.count() > TimeInterval)
	{
		Delta120 += chrono::milliseconds((int)TimeInterval);
		dynamicsWorld->stepSimulation(1.0f / 120, 10);
		RequestFromAll(PLAYER_INPUT);
		btTransform trans;
		cubes[0]->getMotionState()->getWorldTransform(trans);
		btVector3 origin = trans.getOrigin();

		//printf("%f, %f, %f\n", origin.getX(), origin.getY(), origin.getZ());
		SendCubeMatrix();

		for (Packet = Peer->Receive(); Packet; Packet = Peer->Receive())
		{
			CheckPacket(*Packet);
			Peer->DeallocatePacket(Packet);
		}
	}
}

void Server::CheckPacket(const RakNet::Packet& P)
{
	switch (P.data[0])
	{
	case ID_NEW_INCOMING_CONNECTION:
		Connections->ConnectUser(Packet);
		CONSOLE(Packet->guid.ToString() << " Connected to server");
		break;
	case USERNAME_FOR_GUID:
		Result = Connections->RegisterGuid(Packet);
		if (Result == "RECONNECT") { SendResponse(Packet->systemAddress, LOGIN_ACCEPTED); CONSOLE("ID: " << Packet->guid.ToString() << " reconnected"); break; }
		if (Result == "NONE") { SendResponse(Packet->systemAddress, LOGIN_FAILED); CONSOLE("ID: " << Packet->guid.ToString() << " failed to give username"); break; }
		else 
		{
			SendResponse(Packet->systemAddress, LOGIN_ACCEPTED);
			CONSOLE(Packet->guid.ToString() << " gave an username " << Result);
		}
		break;
	case ID_CONNECTION_LOST:
		Connections->RemoveUser(Packet);
		CONSOLE(Packet->guid.ToString() << " Connection lost");
		break;
	case PLAYER_COORD:
		ReadPlayerCoord(Packet);
		break;
	case PLAYER_INPUT:
		ReadPlayerInput(Packet);
		break;
	}
}

void Server::BroadcastVar(CustomMessages Var, RakNet::Packet Packet)
{
	RakNet::BitStream bs(Packet.data,Packet.length,false);
	Peer->Send(&bs, MEDIUM_PRIORITY, RELIABLE_ORDERED, 0, Packet.systemAddress, true, 0);
}

void Server::ReadPlayerCoord(RakNet::Packet* packet)
{
	RakNet::BitStream bs(packet->data,packet->length,false);
	bs.IgnoreBytes(sizeof(RakNet::MessageID));

	int x;
	int y;

	bs.Read(x);
	bs.Read(y);
	std::string user = Connections->FindUsername(packet->guid.ToString());
	if (user != "NONE")
	{
		CONSOLE("Received " << x << ", " << y << " From: " << user);
	}
}

void Server::ReadPlayerInput(RakNet::Packet* packet)
{
	RakNet::BitStream bs(packet->data, packet->length, false);
	bs.IgnoreBytes(sizeof(RakNet::MessageID));

	int w = 0;
	int a = 0;
	int s = 0;
	int d = 0;
	bs.Read(w);
	bs.Read(a);
	bs.Read(s);
	bs.Read(d);
	//TODO(mika) handlaa inputit;
	//printf("w: %i, a: %i, s: %i, d: %i, from: %s\n",w,a,s,d,packet->guid.ToString());
}

void Server::SendResponse(RakNet::SystemAddress sys, CustomMessages responseID)
{
	RakNet::BitStream bs;
	bs.Write((MessageID)responseID);
	Peer->Send(&bs, MEDIUM_PRIORITY, RELIABLE_ORDERED, 0, sys, false, 0);
}

bool Server::AskForVariable(CustomMessages var, INT64 guid)
{
	RakNet::BitStream bs;
	RakNet::RakNetGUID rakguid;
	rakguid.FromString(to_string(guid).c_str());

	bs.Write((RakNet::MessageID)var);
	Peer->Send(&bs, MEDIUM_PRIORITY, RELIABLE_ORDERED, 0, rakguid, false, 0);
	return true;
}

bool Server::AskForVariable(CustomMessages var, string username)
{
	string guid = Connections->FindGuid(username);
	if (guid == "NONE")
	{
		return false;
	}
	INT64 guidint = stoll(guid);
	return AskForVariable(var, guidint);
}

void Server::RequestFromAll(CustomMessages Requested)
{
	RakNet::BitStream bs;
	bs.Write((RakNet::MessageID)Requested);
	Peer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, true, 0);
}

void Server::SendCubeMatrix()
{
	RakNet::BitStream bs;
	btTransform trans;
	btVector3 origin;
	btScalar matrix;
	bs.Write((RakNet::MessageID)CUBE_INFO);
	bs.Write(cubes.size());
	for (int i; i < cubes.size(); i++)
	{
		cubes[i]->getMotionState()->getWorldTransform(trans);
		trans.getOpenGLMatrix(&matrix);
		bs.Write(matrix);
	}
	Peer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, true, 0);
}

