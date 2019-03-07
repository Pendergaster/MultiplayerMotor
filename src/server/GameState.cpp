#include "GameState.h"



GameState::GameState()
{
	this->ball.position = Vec2(0, 0);
	this->ball.velocity = Vec2(0, 1);
	this->ball.radius = 0.25f;
	AddPlayer(1, "vasuri");
	AddPlayer(2, "oikuri");
}


GameState::~GameState()
{
	players.clear();
}

void GameState::AddPlayer(int guid,const char* name)
{
	Vec2 playerPos;
	if (playersLeft <= playersRight)
	{
		playersLeft++;
		playerPos = Vec2(-10, 0);
	}
	else
	{
		playersRight++;
		playerPos = Vec2(10, 0);
	}
	players.push_back(new Player(name,guid,playerPos));
}


void GameState::RemovePlayer(int guid)
{
	Player* result = FindPlayer(guid);
	if (result == nullptr)
	{
		std::cout << "Could not find player with guid " << guid << std::endl;
		return;
	}
	std::cout << "Removing player: " << result->name << std::endl;

	for (int i = 0; i < players.size(); i++)
	{
		if (players[i] == result)
		{
			players.erase(players.begin() + i);
		}
	}
}

void GameState::Tick()
{
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	std::chrono::milliseconds millis = std::chrono::duration_cast<std::chrono::milliseconds>(now - previousTime);
	previousTime = now;
	float tick = millis.count();
	UpdatePlayerPos(tick);
	UpdateBallPos(tick);
	//std::cout << ball.position.x << " " << ball.position.y << std::endl;
}

void GameState::UpdateBallPos(double tick)
{
	Vec2 vel = ball.velocity / tick;
	ball.position += vel;
	CheckCollision();
}
void GameState::CheckCollision()
{
	for (Player* var : players)
	{
		float diffX = var->position.x - ball.position.x;
		float diffY = var->position.y - ball.position.y;
		float distance = sqrt(diffX * diffX + diffY*diffY);
		if (distance < ball.radius) 
		{
			//TODO: add in player width;
			ball.velocity.x *= -1; 
		}
	}
	if (ball.position.y + ball.radius > 10)
	{
		ball.velocity.y *= -1;
	}
	if (ball.position.y - ball.radius < -10)
	{
		ball.velocity.y *= -1;
	}
	if (ball.position.x > 11) { std::cout << "Left side wins" << std::endl; }
	if (ball.position.x < -11) { std::cout << "Right side wins" << std::endl; }
}

void GameState::ProcessInput(int guid, Vec2 input) //Wasd is replaceable
{
	Player* result = FindPlayer(guid);
	if (result == nullptr)
	{
		std::cout << "Could not find player with guid " << guid << std::endl;
		return;
	}
	if (input.x) { result->velocity += 0.25f; }
	if (input.y) { result->velocity -= 0.25f; }
}

void GameState::UpdatePlayerPos(double dt)
{
	//Updatetaan pelaajien positiotiota heidän velocityn mukaan
	//delta time mukaan	
	for (Player* var : players)
	{
		var->position.y += var->velocity / dt;
	}
}


Player* GameState::FindPlayer(int guid)
{
	for (Player* playerVar : players)
	{
		playerVar->guid = guid;
		return playerVar;
	}
	return nullptr;
}
