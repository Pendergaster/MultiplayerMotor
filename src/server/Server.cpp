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
	btBoxShape* platform = new btBoxShape({100,1,100});
	btMotionState* motion = new btDefaultMotionState(t);
	btRigidBody::btRigidBodyConstructionInfo info(0.0, motion, platform);
	floor = new btRigidBody(info);

	btVector3 defaultIntertia(0, 0, 0);

	dynamicsWorld->addRigidBody(floor);

	for (int x = 0; x < 3; x++) //tehdään cubet mappiin
	{
		for (int y = 0; y < 3; y++) //tehdään cubet mappiin
		{
			for (int z = 0; z < 3; z++) //tehdään cubet mappiin
			{
				AddCube(1, { (float)x,(float)y,(float)z }, { 0.0f,0.0f,0.0f });
				//t.setOrigin({ (float)x,(float)z,(float)y });
				//btBoxShape* box = new btBoxShape({ 0.5,0.5,0.5 });
				//box->calculateLocalInertia(1.0f, defaultIntertia);
				//btMotionState* boxmotion = new btDefaultMotionState(t);
				//btRigidBody::btRigidBodyConstructionInfo boxInfo(0.2f, boxmotion, box, defaultIntertia);
				//btRigidBody* newBox = new btRigidBody(boxInfo);
				//newBox->setFriction(0.1f);
				//cubes.push_back(newBox);
				//dynamicsWorld->addRigidBody(newBox);
			}
		}
	}
}

void Server::RemoveSmallCube(int id)
{
	for (int i = 0; i < cubes.size(); i++)
	{
		if (smallCubesActive[i].id == id)
		{
			smallCubesActive[i].id = smallCubesActive.size() + 1;
			smallCubesActive[i].rb->clearForces();
			smallCubesActive[i].rb->forceActivationState(WANTS_DEACTIVATION);
			smallCubesInactive.push_back(smallCubesActive[i]);
			smallCubesActive.erase(smallCubesActive.begin() + i);
		}
	}
}

void Server::AddCube(int type, vec3 pos, vec3 rot)
{

	if (smallCubesInactive.size() == 0) //Create new rigidbody for new cube
	{
		btTransform t;
		t.setIdentity();
		t.setOrigin({pos.x,pos.y,pos.z});


		btVector3 defaultIntertia(0, 0, 0);
		btBoxShape* box = new btBoxShape({ 0.5,0.5,0.5 });
		box->calculateLocalInertia(0.2f, defaultIntertia);

		btMotionState* boxmotion = new btDefaultMotionState(t);
		btRigidBody::btRigidBodyConstructionInfo boxInfo(0.2f, boxmotion, box, defaultIntertia);
		btRigidBody* newBox = new btRigidBody(boxInfo);

		newBox->setFriction(0.1f);
		dynamicsWorld->addRigidBody(newBox);

		smallCubesActive.push_back(Cube(smallCubesActive.size() + 1, type, newBox));
	}
	else
	{
		smallCubesActive.push_back(smallCubesInactive[0]); //copy from first inactive to active ones
		smallCubesActive[smallCubesActive.size() - 1].rb->forceActivationState(ACTIVE_TAG);
		smallCubesInactive.erase(smallCubesInactive.begin()); 
	}
}

void Server::AddPlayerCube(std::string name)
{
	btVector3 defaultIntertia(0, 0, 0);

	btBoxShape* box = new btBoxShape({ 2,2,2 });
	box->calculateLocalInertia(1.0f, defaultIntertia);

	btTransform t;
	t.setIdentity();
	t.setOrigin({0,5,0});

	btMotionState* boxmotion = new btDefaultMotionState(t);
	btRigidBody::btRigidBodyConstructionInfo boxInfo(20.0f, boxmotion, box, defaultIntertia);
	btRigidBody* newBox = new btRigidBody(boxInfo);
	newBox->setFriction(0.1f);
	players.push_back(newBox);
	slots.push_back(name);
	dynamicsWorld->addRigidBody(newBox);
}

void Server::RemovePlayerCube(std::string name)
{
	if (players.size() != 0)
	{
		for (int i = 0; i < players.size(); i++)
		{
			if (slots[i] == name)
			{
				slots.erase(slots.begin() + i);
				players.erase(players.begin() + i);
			}
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
	TimeInterval = (int)((1.0 / 120) * 1000);

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
		dynamicsWorld->stepSimulation(1.0 / 120.0);

		//SendCubeInfo();
		WriteBulk();

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
		//if (Result == "RECONNECT") { SendResponse(Packet->systemAddress, LOGIN_ACCEPTED); CONSOLE("ID: " << Packet->guid.ToString() << " reconnected"); break; }
		//if (Result == "NONE") { SendResponse(Packet->systemAddress, LOGIN_FAILED); CONSOLE("ID: " << Packet->guid.ToString() << " failed to give username"); break; }
		//else 
		//{
		SendResponse(Packet->systemAddress, LOGIN_ACCEPTED);
		AddPlayerCube(Packet->guid.ToString());
		SendSlotID(Packet->systemAddress, players.size()-1);
		CONSOLE(Packet->guid.ToString() << " gave an username " << Result);
		//}
		break;
	case ID_CONNECTION_LOST:
		Connections->RemoveUser(Packet);
		RemovePlayerCube(Packet->guid.ToString());
		CONSOLE(Packet->guid.ToString() << " Connection lost");
		break;
	case PLAYER_COORD:
		ReadPlayerCoord(Packet);
		break;
	case PLAYER_INPUT:
		ReadPlayerInput(Packet);
		break;
	case PLAYER_STATE:
		ReadPlayerState(Packet);
		break;
	case PLAYER_LOOK_DIR:
		ReadPlayerLookDir(Packet);
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
	if (players.size() != 0)
	{
		//std::string target = Connections->FindUsername(packet->guid.ToString());
		for (int i = 0; i < players.size(); i++)
		{
			if (slots[i] == packet->guid.ToString())
			{
				players[i]->activate(true);
				players[i]->applyCentralForce({ (float)((a-d)*20),0,(float)((w-s)*20) });
			}
		}
	}
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

void Server::SendSlotID(RakNet::SystemAddress addr, int id)
{
	RakNet::BitStream bs;
	bs.Write((MessageID)PLAYER_SLOT);
	bs.Write(id);
	Peer->Send(&bs, MEDIUM_PRIORITY, RELIABLE_ORDERED, 0, addr, false, 0);
}

void Server::SendCubeInfo()
{
	RakNet::BitStream bs;
	btTransform trans;
	bs.Write((RakNet::MessageID)CUBE_INFO);
	int size = cubes.size();
	bs.Write(size);
	for (int i = 0; i < cubes.size(); i++)
	{
		cubes[i]->getMotionState()->getWorldTransform(trans);
		bs.Write(trans.getOrigin());
		bs.Write(trans.getRotation());
	}
	Peer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, true, 0);
}

void Server::ReadPlayerState(RakNet::Packet* packet)
{
	RakNet::BitStream bs(packet->data, packet->length, false);
	bs.IgnoreBytes(sizeof(RakNet::MessageID));

	vec3 lookDir;
	inputType input;
	bs.Read(lookDir);
	bs.Read(input);
	//printf("%i\n", input);
	UpdatePlayerCube(packet->guid.ToString(),input,lookDir);
	//extract input
	//extract lookdir
	//Handle input

}

void Server::UpdatePlayerCube(std::string guid, inputType input, vec3 lookDir)
{
	//W = 22, A = 0, S = 18, D = 3
	//Check Input
	for (int i = 0; i < players.size(); i++)
	{
		if (slots[i] == guid)
		{
			players[i]->activate(true);
			vec3 cross = cross_product(lookDir, { 0,1,0 });
			cross = normalized(cross);

			lookDir = lookDir * MovementSpeedMultiplier;
			cross = cross * MovementSpeedMultiplier;

			if ((input & (1 << 22)) != 0)
			{
				//printf("w\n");
				players[i]->applyCentralForce({lookDir.x,0,lookDir.z});
			}
			if ((input & (1 << 18)) != 0)
			{
				//printf("s\n");
				players[i]->applyCentralForce({-lookDir.x,0,-lookDir.z});
			}
			if ((input & (1 << 0)) != 0)
			{
				//printf("a\n");
				players[i]->applyCentralForce({-cross.x,0,-cross.z});
			}
			if ((input & (1 << 3)) != 0)
			{
				//printf("d\n");
				players[i]->applyCentralForce({cross.x,0,cross.z});
			}
		}
	}
}

void Server::WriteBulk()
{
	RakNet::BitStream bs;
	btTransform trans;

	bs.Write((RakNet::MessageID)READ_BULK);
	//adding cubeinfo data to packet;

	if (smallCubesActive.size() != 0)
	{
		bs.Write((RakNet::MessageID)CUBE_INFO);
		//int size = cubes.size();
		//bs.Write(size);
		//for (int i = 0; i < cubes.size(); i++)
		//{
		//	cubes[i]->getMotionState()->getWorldTransform(trans);
		//	bs.Write(trans.getOrigin());
		//	bs.Write(trans.getRotation());
		//}

		int size = smallCubesActive.size();
		bs.Write(size);

		for (int i = 0; i < size; i++)
		{
			bs.Write(smallCubesActive[i].id);
			bs.Write(smallCubesActive[i].Type);

			smallCubesActive[i].rb->getMotionState()->getWorldTransform(trans);
			bs.Write(trans.getOrigin());
			bs.Write(trans.getRotation());
		}
	}

	if (players.size() != 0)
	{
		bs.Write((RakNet::MessageID)PLAYER_INFO);
		int size = players.size();
		bs.Write(size);
		for (int i = 0; i < players.size(); i++)
		{
			players[i]->getMotionState()->getWorldTransform(trans);
			bs.Write(trans.getOrigin());
			bs.Write(trans.getRotation());
		}
	}
	Peer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, true, 0);
}

void Server::ReadPlayerLookDir(RakNet::Packet* packet)
{
	RakNet::BitStream bs(packet->data, packet->length, false);
	bs.IgnoreBytes(sizeof(RakNet::MessageID));

	for (int i = 0; i < slots.size(); i++)
	{
		if(slots[i] == packet->guid.ToString())
		{

		}
	}
}

void Server::SendSmallCubeInfo()
{
	RakNet::BitStream bs;
	btTransform trans;
	bs.Write((RakNet::MessageID)CUBE_INFO);
	int size = smallCubesActive.size();
	bs.Write(size);

	for (int i = 0; i < size; i++)
	{
		bs.Write(smallCubesActive[i].id);
		bs.Write(smallCubesActive[i].Type);

		smallCubesActive[i].rb->getMotionState()->getWorldTransform(trans);
		bs.Write(trans.getOrigin());
		bs.Write(trans.getRotation());
	}

	//for (int i = 0; i < cubes.size(); i++)
	//{
	//	cubes[i]->getMotionState()->getWorldTransform(trans);
	//	bs.Write(trans.getOrigin());
	//	bs.Write(trans.getRotation());
	//}
	Peer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, true, 0);


}
