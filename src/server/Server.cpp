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
	PlayerSlot = vector<PlayerData>(4);
	playerAmount = 0;
	InitBulletWorld();
}

void Server::InitBulletWorld()
{
	collisionConfiguration =	new btDefaultCollisionConfiguration();
	dispatcher =				new btCollisionDispatcher(collisionConfiguration);
	overlappingPairCache =		new btDbvtBroadphase();
	solver =					new  btSequentialImpulseConstraintSolver;
	dynamicsWorld =				new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

	dynamicsWorld->setGravity(btVector3(physics_gravity.x,physics_gravity.y,physics_gravity.z));

	AddCube(ObjectType::Floor, { 0.0f,-4.0f,0.0f }, { 0.0f,0.0f,0.0f });
	AddCube(ObjectType::Floor, { -20.0f,-4.0f,0.0f }, { 0.0f,0.0f,0.0f });
	AddCube(ObjectType::Floor, { 0.0f,-4.0f,-20.0f }, { 0.0f,0.0f,0.0f });
	AddCube(ObjectType::Floor, { 20.0f,-4.0f,0.0f }, { 0.0f,0.0f,0.0f });
	AddCube(ObjectType::Floor, { 0.0f,-4.0f,20.0f }, { 0.0f,0.0f,0.0f });

	for (int x = 0; x < 3; x++) //tehdään cubet mappiin
	{
		for (int y = 0; y < 3; y++) //tehdään cubet mappiin
		{
			for (int z = 0; z < 3; z++) //tehdään cubet mappiin
			{
				AddCube(ObjectType::FreeSimulation, { (float)x ,(float)y + 50.0f,(float)z }, { 0.0f,0.0f,0.0f });
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
			smallCubesActive[i].id = (int)smallCubesActive.size() + 1;
			smallCubesActive[i].rb->clearForces();
			smallCubesActive[i].rb->forceActivationState(WANTS_DEACTIVATION);
			smallCubesInactive.push_back(smallCubesActive[i]);
			smallCubesActive.erase(smallCubesActive.begin() + i);

		}
	}
}

void Server::AddCube(ObjectType type, vec3 pos, vec3 rot)
{
	if (type == ObjectType::Player)
	{
		btTransform t;
		t.setIdentity();
		t.setOrigin({ pos.x,pos.y,pos.z });

		btVector3 defaultIntertia(0, 0, 0);
		btBoxShape* box = new btBoxShape({ player_scale.x, player_scale.y, player_scale.z });
		box->calculateLocalInertia(free_mass*5, defaultIntertia);

		btMotionState* boxmotion = new btDefaultMotionState(t);
		btRigidBody::btRigidBodyConstructionInfo boxInfo(free_mass*5, boxmotion, box, defaultIntertia);
		btRigidBody* newBox = new btRigidBody(boxInfo);
		newBox->setAngularFactor({ 0,1,0 });
		newBox->setFriction(0.0f);
		newBox->setSpinningFriction(0);
		newBox->setDamping(0.5f, 0.5f);
		dynamicsWorld->addRigidBody(newBox);
		Players.push_back(Cube(Players.size()+Floors.size()+smallCubesActive.size() + 1, type, newBox));
	}
	if (type == ObjectType::Floor)
	{
		btTransform t;
		t.setIdentity();
		t.setOrigin({ pos.x,pos.y,pos.z });

		btVector3 defaultIntertia(0, 0, 0);
		btBoxShape* box = new btBoxShape({ floor_scale.x, floor_scale.y, floor_scale.z });
		box->calculateLocalInertia(0.0f, defaultIntertia);

		btMotionState* boxmotion = new btDefaultMotionState(t);
		btRigidBody::btRigidBodyConstructionInfo boxInfo(0.0f, boxmotion, box, defaultIntertia);
		btRigidBody* newBox = new btRigidBody(boxInfo);


		newBox->setFriction(floor_friction);
		dynamicsWorld->addRigidBody(newBox);
		Floors.push_back(Cube(Players.size()+Floors.size()+smallCubesActive.size() + 1, type, newBox));
		
	}
	if (type == ObjectType::FreeSimulation)
	{
		if (smallCubesInactive.size() == 0) //Create new rigidbody for new cube
		{
			btTransform t;
			t.setIdentity();
			t.setOrigin({pos.x,pos.y,pos.z});

			btVector3 defaultIntertia(0, 0, 0);
			btBoxShape* box = new btBoxShape({free_scale.x, free_scale.y, free_scale.z});
			box->calculateLocalInertia(free_mass, defaultIntertia);

			btMotionState* boxmotion = new btDefaultMotionState(t);
			btRigidBody::btRigidBodyConstructionInfo boxInfo(free_mass, boxmotion, box, defaultIntertia);
			btRigidBody* newBox = new btRigidBody(boxInfo);

			//newBox->setFriction(0.1f);
			dynamicsWorld->addRigidBody(newBox);

			smallCubesActive.push_back(Cube(Players.size()+Floors.size()+smallCubesActive.size() + 1, type, newBox));
		}
		else
		{
			smallCubesActive.push_back(smallCubesInactive[0]); //copy from first inactive to active ones
			smallCubesActive[smallCubesActive.size() - 1].rb->forceActivationState(ACTIVE_TAG);
			smallCubesInactive.erase(smallCubesInactive.begin()); 
		}
	}
}

void Server::AddPlayerCube(std::string name)
{
	for(int i = 0; i < 4; i++)
	{
		if(PlayerSlot[i].name[0] == '\0')
		{
			switch (i)
			{
			case 0:
				AddCube(ObjectType::Player, { -10,3,0 }, { 0,0,0 }); //We create cube for player
				break;
			case 1:
				AddCube(ObjectType::Player, { 0,3,-10 }, { 0,0,0 }); //We create cube for player
				break;
			case 2:
				AddCube(ObjectType::Player, { 10,3,0 }, { 0,0,0 }); //We create cube for player
				break;
			case 3:
				AddCube(ObjectType::Player, { 0,3,10 }, { 0,0,0 }); //We create cube for player
				break;
			}

			strcpy(PlayerSlot[i].name, name.c_str());
			PlayerSlot[i].playerId = i;
			PlayerSlot[i].kuutioId = Players.back().id;
			PlayerSlot[i].score = 0;
			playerAmount++;
			break;
		}
	}
}

void Server::RemovePlayerCube(std::string name)
{
	for (int i = 0; i < 4; i++)
	{
		if(strcmp(PlayerSlot[i].name,name.c_str()))
		{
			PlayerSlot[i].name[0] = '\0';
			playerAmount--;
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
	TimeInterval = (int)((1.0 / 30.0) * 1000000.0);

	running = true;
}

void Server::ServerStop()
{
	running = false;
	RakPeerInterface::DestroyInstance(Peer);
	delete Connections;
}

void Server::ServerUpdate()
{
	auto Delta = chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now() - Delta120);
	/*Loads packet from peer*/
	if ((float)Delta.count() > TimeInterval)
	{
		{
			// Record start time
			static auto last = std::chrono::high_resolution_clock::now();
			// Record end time
			auto current = std::chrono::high_resolution_clock::now();

			std::chrono::duration<double> elapsed = current - last;
			std::cout << "TIME " << elapsed.count() << std::endl;
			//last = current;
			serverDelta = elapsed.count();
		}

		Delta120 += chrono::microseconds((int)TimeInterval);
		//serverDelta = (double)Delta.count() / 1000000.0; //Kuinka kauan serverillä kesti päästä update funktioon
		//serverDelta = (double)Delta.count() / 1000000; //Kuinka kauan serverillä kesti päästä update funktioon
		//std::cout << "took " << serverDelta << " seconds" << endl;
		dynamicsWorld->stepSimulation(1.0 / 30.0,8);
		//SendCubeInfo();
		WriteBulk();
		SendPlayerInfo();
		CheckCubes();

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
		SendResponse(Packet->systemAddress, LOGIN_ACCEPTED);
		slots.push_back(Packet->guid.ToString()); //we save its guid.
		AddPlayerCube(Result);
		CONSOLE(Packet->guid.ToString() << " gave an username " << Result);
		break;
	case ID_CONNECTION_LOST:
		Connections->RemoveUser(Packet);
		RemovePlayerCube(Packet->guid.ToString());
		CONSOLE(Packet->guid.ToString() << " Connection lost");
		break;
	case PLAYER_STATE:
		ReadPlayerState(Packet);
		break;
	}
}

void Server::SendResponse(RakNet::SystemAddress sys, CustomMessages responseID)
{
	RakNet::BitStream bs;
	bs.Write((MessageID)responseID);
	Peer->Send(&bs, MEDIUM_PRIORITY, RELIABLE_ORDERED, 0, sys, false, 0);
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
	Input input;
	bs.Read(input);
	UpdatePlayerCube(packet->guid.ToString(),input);
}

void Server::UpdatePlayerCube(std::string guid, Input playerInput)
{
	//W = 22, A = 0, S = 18, D = 3
	//Check Input
	for (int i = 0; i < Players.size(); i++)
	{
		if (slots[i] == guid)
		{
			Players[i].rb->activate(true);
			inputType input = playerInput.keys;
			btTransform trans;
			vec3 target(0, 0, 1);
			Players[i].rb->getMotionState()->getWorldTransform(trans);
			btQuaternion rotation = trans.getRotation();
			quaternion newRotation(rotation.getW(),rotation.getX(), rotation.getY(), rotation.getZ());
			target = rotate_vector_by_quaternion(target,newRotation);

			vec3 cross = cross_product(target, { 0,1,0 });
			cross = normalized(cross);

			target = target * MovementSpeedMultiplier;
			cross = cross * MovementSpeedMultiplier;

			float yaw = playerInput.lastmpos.x - playerInput.mpos.x;

			if ((input & (1 << 22)) != 0)
			{
				//printf("w\n");
				Players[i].rb->applyCentralForce({ target.x,0, target.z});
			}
			if ((input & (1 << 18)) != 0)
			{
				//printf("s\n");
				Players[i].rb->applyCentralForce({-target.x,0,-target.z});
			}
			if ((input & (1 << 0)) != 0)
			{
				//printf("a\n");
				Players[i].rb->applyCentralForce({ -cross.x,0, -cross.z});
			}
			if ((input & (1 << 3)) != 0)
			{
				//printf("d\n");
				Players[i].rb->applyCentralForce({cross.x,0,cross.z});
			}
			Players[i].rb->applyTorque({ 0,(yaw*turningSpeedMultiplier),0 });
		}
	}
}

void Server::WriteBulk()
{
	RakNet::BitStream bs;
	btTransform trans;

	bs.Write((RakNet::MessageID)READ_BULK);
	//adding cubeinfo data to packet;
	packetID++;
	if (packetID > 99999) packetID = 0;
	bs.Write(packetID);
	
	if (smallCubesActive.size() != 0 || Floors.size() != 0 || Players.size() != 0)
	{
		bs.Write((RakNet::MessageID)CUBE_INFO);
		
		int size = (int)smallCubesActive.size() + (int)Floors.size() + (int)Players.size();
		bs.Write(size);

		for (int i = 0; i < Floors.size(); i++)
		{
			bs.Write(Floors[i].id);
			bs.Write(Floors[i].type);

			Floors[i].rb->getMotionState()->getWorldTransform(trans);
			bs.Write(trans.getOrigin());
			bs.Write(trans.getRotation());
		}
		for (int i = 0; i < smallCubesActive.size(); i++)
		{
			bs.Write(smallCubesActive[i].id);
			bs.Write(smallCubesActive[i].type);

			smallCubesActive[i].rb->getMotionState()->getWorldTransform(trans);
			bs.Write(trans.getOrigin());
			bs.Write(trans.getRotation());
		}
		for (int i = 0; i < Players.size(); i++)
		{
			bs.Write(Players[i].id);
			bs.Write(Players[i].type);

			Players[i].rb->getMotionState()->getWorldTransform(trans);
			bs.Write(trans.getOrigin());
			bs.Write(trans.getRotation());
			//bs.Write(Players[i].rb->getLinearVelocity()); //TODO: ehkä tämäkin.
		}
	}
	bs.Write((RakNet::MessageID)SERVER_DELTA);
	bs.Write(serverDelta);
	Peer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, true, 0);
}

void Server::SendPlayerInfo()
{
	RakNet::BitStream bs;
	bs.Write((MessageID)PLAYER_INFO);
	bs.Write(playerAmount);

	for (int i = 0; i < 4; i++)
	{
		if (PlayerSlot[i].name[0] != '\0')
		{
			bs.Write(PlayerSlot[i]);
		}
	}
	if (playerAmount != 0)
	{
		Peer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, true, 0);
	}
}

void Server::SendSmallCubeInfo()
{
	RakNet::BitStream bs;
	btTransform trans;
	bs.Write((RakNet::MessageID)CUBE_INFO);
	size_t size = smallCubesActive.size();
	bs.Write(size);

	for (int i = 0; i < size; i++)
	{
		bs.Write(smallCubesActive[i].id);
		bs.Write(smallCubesActive[i].type);

		smallCubesActive[i].rb->getMotionState()->getWorldTransform(trans);
		bs.Write(trans.getOrigin());
		bs.Write(trans.getRotation());
	}

	Peer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, true, 0);
}

void Server::CheckCubes()
{
	for (int i = 0; i < PlayerSlot.size(); i++)
	{
		PlayerSlot[i].score = 0;
	}

	btTransform trans;
	btVector3 newpos(0,5,0);
	for (int i = 0; i < smallCubesActive.size(); i++)
	{
		smallCubesActive[i].rb->getMotionState()->getWorldTransform(trans);

		if (trans.getOrigin().getX() < -10)
		{
			PlayerSlot[0].score++;
		}
		else if (trans.getOrigin().getZ() < -10)
		{
			PlayerSlot[1].score++;
		}
		else if (trans.getOrigin().getX() > 10)
		{
			PlayerSlot[2].score++;
		}
		else if (trans.getOrigin().getZ() > 10)
		{
			PlayerSlot[3].score++;
		}

		if (trans.getOrigin().getY() < -5)
		{
			newpos.setX((rand() % 200)/10.0f - 10.0f);
			newpos.setZ((rand() % 200)/10.0f - 10.0f);
			trans.setOrigin(newpos);
			trans.setRotation({ 0,0,0 });
			smallCubesActive[i].rb->setWorldTransform(trans);
			smallCubesActive[i].rb->clearForces();
			smallCubesActive[i].rb->setLinearVelocity({ 0,0,0 });
			smallCubesActive[i].rb->setAngularVelocity({ 0,0,0 });
		}
	}
	for (int i = 0; i < Players.size(); i++)
	{
		Players[i].rb->getMotionState()->getWorldTransform(trans);
		if (trans.getOrigin().getY() < -5)
		{
			trans.setOrigin(newpos);
			trans.setRotation({ 0,0,0 });
			Players[i].rb->setWorldTransform(trans);
			Players[i].rb->clearForces();
			Players[i].rb->setLinearVelocity({ 0,0,0 });
			Players[i].rb->setAngularVelocity({ 0,0,0 });
		}
	}
}
