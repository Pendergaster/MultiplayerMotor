#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <random>
#include <chrono>

struct Vec2
{
	Vec2(float x, float y) { this->x = x; this->y = y; }
	Vec2() {};
	float x;
	float y;

	Vec2 operator=(Vec2 add)
	{
		this->x = add.x;
		this->y = add.y;
		return Vec2(this->x,this->y);
	}
	Vec2 operator+=(Vec2 add)
	{
		this->x += add.x;
		this->y += add.y;
		return Vec2(this->x, this->y);
	}
	Vec2 operator+(Vec2 add) { return Vec2(x + add.x, y + add.y); }
	Vec2 operator-(Vec2 add) { return Vec2(x - add.x, y - add.y); }
	Vec2 operator*(Vec2 add) { return Vec2(x * add.x, y * add.y); }
	Vec2 operator*(float add) { return Vec2(x * add, y * add); }
	Vec2 operator/(float divider) 
	{ 
		float x = this->x / divider;
		float y = this->y / divider;
		return Vec2(x,y); 
	}
	Vec2 operator*=(float add)
	{
		this->x *= add;
		this->y *= add;
		return Vec2(x, y);
	}
};

struct Vec3
{
	int x;
	int y;
	int z;

	static Vec3 randomize() 
	{
		Vec3 tmp;
		tmp.x = std::rand() % 100;
		tmp.y = std::rand() % 100;
		tmp.z = std::rand() % 100;
		return tmp;
	}
};

struct Player
{
	Player(const char* name, int guid, Vec2 position) { this->name = name; this->guid = guid; this->position = position; this->color = Vec3::randomize(); }
	std::string name;
	int guid;
	Vec2 position;
	int velocity = 0;
	Vec3 color;
};

struct Ball
{
	Vec2 position;
	Vec2 velocity;
	float radius;
};

class GameState
{
	/*Public Functions*/
public:
	GameState();
	~GameState();

	void AddPlayer(int guid, const char* name);
	void RemovePlayer(int guid);

	Vec2 GetBallPos() { return ball.position; }
	void ProcessInput(int guid, Vec2 input); //Vec2 acts as input. x = W, y = S;
	void Tick();

	/*Private Functions*/
private:
	Player* FindPlayer(int guid);
	void UpdatePlayerPos(double dt);
	void UpdateBallPos(double dt);
	void CheckCollision();
	
	/*Public Variables*/
public:

	/*Private Variables*/
private:	
	int playersLeft = 0;
	int playersRight = 0;
	std::vector<Player*> players;
	Ball ball;

	std::chrono::system_clock::time_point previousTime = std::chrono::system_clock::now();
};

