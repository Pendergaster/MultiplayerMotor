#include <iostream>
#include "Server.h"
#include "Server.cpp"


int main()
{
	Server* Connection = new Server("127.0.0.1", 60000);
	bool Running = true;


	while (Running)
	{
		Connection->ServerUpdate();
	}
	delete Connection;
	return 0;
}