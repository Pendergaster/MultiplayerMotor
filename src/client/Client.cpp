#include "Client.h"

using namespace RakNet;
using namespace std;

void Client::Init(std::string IP, int Port, const char* username)
{
	m_ip = std::string(IP);
	//this->m_ip = IP;
	//this->m_ip = "127.0.0.1";
	this->m_port = Port;
	strcpy(m_username, username);
	//this->m_username = username;
	this->Peer = RakNet::RakPeerInterface::GetInstance();
	this->SD = RakNet::SocketDescriptor(0, 0);

	string title = "Raknet-Client";
	SetConsoleTitle(title.c_str());

}

Client::~Client()
{
}

void Client::CloseConnection()
{
	RakPeerInterface::DestroyInstance(Peer);
}

/*Does simple setup for client connection*/
void Client::OpenConnection()
{
	Peer = RakPeerInterface::GetInstance();

	Peer->Startup(8, &SD, 1);
	Peer->SetOccasionalPing(true);
	Peer->Connect(m_ip.c_str(), (unsigned short)m_port, 0, 0);

	Delta = chrono::system_clock::now();
	TimeInterval = (int)((1.0 / 30) * 1000);
}

CustomMessages Client::Update()
{
	CustomMessages ret = CustomMessages::UNKNOWN;
	//auto TimeDifference = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - Delta);
	/*Loads packet from peer*/
	//if ((float)TimeDifference.count() > TimeInterval)
	//{
		//Delta += chrono::milliseconds((int)TimeInterval);
		SendPlayerState();
		isNewData = false;
		printf("server took %f seconds to update \n", serverDelta);
		for (m_packet = Peer->Receive(); m_packet != 0; m_packet = Peer->Receive())
		{
			/*Switch case that lets us check what kind of packet it was*/
			CustomMessages temp = ClientConnectionUpdate(m_packet);
			if(temp == LOGIN_ACCEPTED) {
				ret = temp;
			}
			Peer->DeallocatePacket(m_packet);
		}
	//}
	return ret;
}

CustomMessages Client::ClientConnectionUpdate(RakNet::Packet* Packet)
{
	CustomMessages ret = CustomMessages::UNKNOWN;
	switch (Packet->data[0])
	{
	case ID_CONNECTION_REQUEST_ACCEPTED:
		HostAddress = Packet->systemAddress;
		previousPacketID = 0;
		CONSOLE("Connection with server at " << m_ip << " was succesful");
		Connected = true;
		SendUsernameForServer();
		break;
	case ID_CONNECTION_LOST:
		CONSOLE("Connection lost to server at " << m_ip);
		Connected = false;
		LoggedIn = false;
		thread(&Client::RetryConnection, this).detach();
		break;
	case PLAYER_COORD:
		CheckForVar(PLAYER_COORD);
		CONSOLE("PLAYER COORD REQUEST RECEIVED");
		break;
	case PLAYER_COORD_UPDATE:
		CONSOLE("RECEIVED NEW COORDS FOR GUID" << Packet->guid.ToString());
		break;
	case LOGIN_ACCEPTED:
		CONSOLE("Server accepted our username: " << m_username);
		LoggedIn = true;
		ret = LOGIN_ACCEPTED;
		break;
	case LOGIN_FAILED:
		CONSOLE("Server did not accept our username");
		//thread(&Client::UsernameChange, this, &m_username).detach();
		LoggedIn = false;
		break;
	case USERNAME:
		CheckForVar(USERNAME);
		CONSOLE("Server is asking for username");
		break;
	case PLAYER_SLOT:
		ReadPlayerSlot(Packet);
		break;
	case PLAYER_INFO:
		ReadPlayerInfo(Packet);
		break;
	case PLAYER_INPUT:
		break;
	case CUBE_INFO:
		//ReadCubeInfo(Packet);
		break;
	case READ_BULK:
		ReadBulk(Packet);
		break;
	}
	return ret;
}

void Client::SendPlayerState()
{
	RakNet::BitStream bs;
	bs.Write((RakNet::MessageID)PLAYER_STATE);
	//printf("%i", input);
	//bs.Write(yaw);
	bs.Write(input);
	Peer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, HostAddress, false, 0);
}

void Client::RetryConnection()
{
	using namespace chrono_literals;
	while (Connected == false)
	{
		CONSOLE("RETRYING CONNECTION");
		Peer->Connect(m_ip.c_str(), (unsigned short)m_port, 0, 0);
		this_thread::sleep_for(10s);
	}
	//thread(&Client::UsernameChange, this).detach();
}

void Client::CheckForVar(CustomMessages messageID)
{
	RakNet::BitStream bs;
	bs.Write((RakNet::MessageID)messageID);
	bool wasregisted = false;
	for (Var<int> var : IntVars)
	{
		if (var.MessageID == messageID)

		{
			wasregisted = true;
			for (int* i : var.Values)
			{
				bs.Write(*i);
			}
		}
	}
	for (Var<std::string> var : StringVars)
	{
		if (var.MessageID == messageID)
		{
			wasregisted = true;
			for (std::string* i : var.Values)
			{
				bs.Write(*i);
			}
		}
	}
	for (Var<float> var : FloatVars)
	{
		if (var.MessageID == messageID)
		{
			wasregisted = true;
			for (float* i : var.Values)
			{
				bs.Write(*i);
			}
		}
	}
	if (wasregisted == true)
	{
		Peer->Send(&bs, MEDIUM_PRIORITY, RELIABLE_ORDERED, 0, HostAddress, false, 0);
	}
}
void Client::ReadPlayerSlot(RakNet::Packet* packet)
{
	RakNet::BitStream bs(packet->data, packet->length, 0);
	bs.IgnoreBytes(sizeof(RakNet::MessageID));

	bs.Read(playerSlot);
}
void Client::ReadPlayerInfo(RakNet::Packet* packet)
{
	RakNet::BitStream bs(packet->data, packet->length, 0);
	bs.IgnoreBytes(sizeof(RakNet::MessageID));

	int size;
	bs.Read(size);

	PlayerData newData;
	Players.clear();
	for (int i = 0; i < size; i++)
	{
		bs.Read(newData);
		//bs.Read(newData.name,12);
		//bs.Read(newData.score);
		//bs.Read(newData.kuutioId);
		//bs.Read(newData.playerId);
		//bs.Read(newData, (unsigned int)sizeof(PlayerData));
		//strcpy(newData.name, name.c_str());

		//bs.Read(name);
		//bs.Read(score);
		//bs.Read(id);
		//bs.Read(playerID);

		Players.push_back(newData);
	}
	//name.clear();
}
void Client::SetVar(CustomMessages MessageID, std::vector<int*> Vars)
{
	Var<int> tmp;
	tmp.type = Type::INT_TYPE;
	tmp.MessageID = MessageID;
	tmp.Values = Vars;
	this->IntVars.push_back(tmp);

	MessageType regType(Type::INT_TYPE, MessageID);
	registeredServerValues.push_back(regType);
}
void Client::SetVar(CustomMessages MessageID, std::vector<float*> Vars)
{
	Var<float> tmp;
	tmp.type = Type::FLOAT_TYPE;
	tmp.MessageID = MessageID;
	tmp.Values = Vars;
	this->FloatVars.push_back(tmp);

	MessageType regType(Type::FLOAT_TYPE, MessageID);
	registeredServerValues.push_back(regType);
}
void Client::SetVar(CustomMessages MessageID, std::vector<string*> Vars)
{
	Var<string> tmp;
	tmp.type = Type::STRING_TYPE;
	tmp.Values = Vars;
	tmp.MessageID = MessageID;
	this->StringVars.push_back(tmp);

	MessageType regType(Type::STRING_TYPE, MessageID);
	registeredServerValues.push_back(regType);
}
void Client::SendUsernameForServer()
{
	RakNet::BitStream BS;
	BS.Write((RakNet::MessageID)USERNAME_FOR_GUID);
	BS.Write(m_username);
	//this->m_username = username;
	//strcpy(m_username, );
	Peer->Send(&BS, MEDIUM_PRIORITY, RELIABLE_ORDERED, 0, HostAddress, false, 0);
}
void Client::ReadCubeInfo(BitStream* bs)
{
	int i = 0;
	bs->Read(i);

	Objects.clear();
	int id = 0;
	ObjectType type;
	btVector3 pos;
	btQuaternion rot;
	btVector3 vel;
	btVector3 anglvel;

	for (int x = 0; x < i; x++)
	{
		bs->Read(id);
		bs->Read(type);
		bs->Read(pos);
		bs->Read(rot);
		//bs->Read(vel);
		//bs->Read(anglvel);

		ObjectTracker newTracker;
		newTracker.pos = { pos.getX(), pos.getY(), pos.getZ() };
		newTracker.orientation.scalar = rot.getW();
		newTracker.orientation.i = rot.getX();
		newTracker.orientation.j = rot.getY();
		newTracker.orientation.k = rot.getZ();

		newTracker.type = type;

		//newTracker.velocity = { vel.getX(), vel.getY(), vel.getZ() };
		//newTracker.angularVelocity = { anglvel.getX(), anglvel.getY(), anglvel.getZ() };

		Objects.push_back(newTracker);
	}
}

void Client::ReadBulk(RakNet::Packet* packet)
{
	RakNet::BitStream bs(packet->data, packet->length, 0);
	bs.IgnoreBytes(sizeof(RakNet::MessageID));

	RakNet::MessageID ID;
	bs.Read(packetID); //Lukee viimeisimmän paketin IDn
	//printf("%i\n", packetID);
	if (previousPacketID < packetID)
	{
<<<<<<< Updated upstream
		if ((packetID - previousPacketID) > 1)
		{
			std::cout << "Packetti missattu " + packetID + ' ' + previousPacketID << endl;
		}
=======
        std::cout << packetID << std::endl;
        if((packetID - previousPacketID) > 1) {
            std::cout << "LKASDJSAKLDJSAKLDJSKA LJD SAKL JSAKL DJS ALKD JSAKL  " << std::endl;
        }

>>>>>>> Stashed changes
		previousPacketID = packetID;
		isNewData = true; //switchi joka flippaa kun on saapunut paketti jossa on uutta dataa
	}

	while (bs.GetNumberOfUnreadBits() != 0)
	{
		bs.Read(ID);
		switch (ID)
		{
		case CUBE_INFO:
			ReadCubeInfo(&bs);
			break;
		case PLAYER_INFO:
			//ReadPlayerInfo(&bs);
			break;
		case SERVER_DELTA:
			bs.Read(serverDelta); //Luetaan kauan servulla kesti updatessa
			break;
		default:
			return;
			break;
		}
	}
}
double Client::GetServerDelta() { return serverDelta; }
