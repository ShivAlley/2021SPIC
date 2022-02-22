#pragma once
#include "common.h"

template <class T>
concept vec2D = requires (T & x) {
	x.x;
	x.y;
};

struct Parameter
{
	Parameter() = default;
	//増築を繰り返したせいで最早templeteの体をなしていない
	template<vec2D v, typename T>
	Parameter(v v1, v v2, T d1, v v3,v v4)
		:maxVelocity(v1)
		, accelerationX(v2.x)
		, jumpPower(v2.y)
		, floorKickPower(d1)
		, wallKickPower(v3)
		, knockbackedToLeft(v4)
		, knockbackedToRight(Vec2(-v4.x, v4.y))
	{}

	Vec2 maxVelocity;
	Vec2 wallKickPower;
	double accelerationX;
	double jumpPower;
	double floorKickPower;
	Vec2 knockbackedToLeft;
	Vec2 knockbackedToRight;
};

class Player //: private Parameter
{
public:
	Player()
		:m_param(
			MAX_VEL,
			ACCELERATION,
			FLOOR_KICK_POWER,
			WALL_KICK_POWER,
			KNOCKBACKED_POWER)
	{}
	P2Body& body() { return m_body; }
	const P2Body& GetBody() const { return m_body; }
	const Parameter& GetParam() const { return m_param; }
	Stopwatch& landingDelay(){ return m_landingDelay; }
	Stopwatch& invincibleTimer(){ return m_invincibleTimer; }
	const bool& GetIsJumpRestriction()const { return m_isJumpRestriction; }
	void SetIsJumpRestriction(bool b) { m_isJumpRestriction = b; }
	const Vec2& GetDeltaVelocity()const { return m_deltaVelocity; }
	void SetDeltaVelocity(Vec2 v) { m_deltaVelocity = v; }
	const Vec2& GetPreviousVelocity()const { return m_previousVelocity; }
	void SetPreviousvelocity(Vec2 v) { m_previousVelocity = v; }
	const bool& GetShouldRecordVelocity()const { return m_shouldRecordVelocity; }
	void SetShouldRecordVelocity(bool b) { m_shouldRecordVelocity = b; }
	const bool& GetIsOnGround()const { return m_isOnGround; }
	void SetIsOnGround(bool b) { m_isOnGround = b; }
	const bool& GetIsInvinvible()const { return m_isInvincible; }
	void SetIsInvincible(bool b) { m_isInvincible = b; }
	const int32& GetHealth()const { return m_health; }
	void DecreaseHealth() { m_health--; }
	void IncreaseHealth() { m_health++; }
	//bool& RefShouldRecordVelocity() { return m_shouldRecordVelocity; }
private:
	P2Body m_body;
	Parameter m_param;
	Stopwatch m_landingDelay{ StartImmediately::No };
	Stopwatch m_invincibleTimer{ StartImmediately::No };
	Vec2 m_deltaVelocity{};
	Vec2 m_previousVelocity{};
	int32 m_health{3};
	bool m_shouldRecordVelocity = true;
	bool m_isJumpRestriction = false;
	bool m_isOnGround = false;
	bool m_isInvincible = false;
	static constexpr Vec2 MAX_VEL = { 1000,1000 };
	static constexpr Vec2 ACCELERATION = { 30000,-100 };
	static constexpr double FLOOR_KICK_POWER = 500;
	static constexpr Vec2 WALL_KICK_POWER = { 500,-500 };
	static constexpr Vec2 KNOCKBACKED_POWER = { -300,-300 };
};




class Enemy
{
public:
	Enemy(){}
	Enemy(P2Body b):m_body(b){}
	virtual ~Enemy(){}
	P2Body& body() { return m_body; }
	const P2Body& GetBody()const { return m_body; }
protected:
	P2Body m_body;
	
private:

};

class WalkingEnemy : public Enemy
{
public:
	WalkingEnemy(P2Body b)
		:Enemy(b)
	{}
	const bool& GetIsLookAtRight()const { return m_isLookAtRight; }
	void ToggleIsLookAtRight() { m_isLookAtRight = not m_isLookAtRight; }
	Stopwatch& turnCooldown() { return m_turnCooldown; }
private:

	bool m_isLookAtRight = true;
	Stopwatch m_turnCooldown{ StartImmediately::Yes };
};

class FlyingEnemy : public Enemy
{
public:
	FlyingEnemy(P2Body b)
		:Enemy(b)
	{}
	const bool& GetIsLookAtDown()const { return m_isLookAtDown; }
	void ToggleIsLookAtDown() { m_isLookAtDown = not m_isLookAtDown; }
	const Stopwatch& GetTurnWatch()const { return m_turnWatch; }
	Stopwatch& turnWatch() { return m_turnWatch; }
	
private:
	bool m_isLookAtDown = true;
	Stopwatch m_turnWatch{ StartImmediately::Yes };
};

class CannonEnemy : public Enemy
{
public:
	CannonEnemy(P2Body b)
		:Enemy(b)
	{}
	Stopwatch& shotInterval() { return m_shotInterval; }
	const Stopwatch& GetShotInterval()const { return m_shotInterval; }
	const bool& GetIsLookAtRight()const { return m_isLookAtRight; }
	void SetIsLookAtRight(bool b) { m_isLookAtRight = b; }

private:
	Stopwatch m_shotInterval{ StartImmediately::Yes };
	bool m_isLookAtRight = false;
};

class BulletEnemy : public Enemy
{
public:
	BulletEnemy(P2Body b,Vec2 v)
		:Enemy(b)
		,m_velocity(v)
	{}
	const Vec2& GetVelocity()const { return m_velocity; }
	void SetVelocity(Vec2 v) { m_velocity = v; }
private:
	Vec2 m_velocity;
};

struct KilledEnemy
{
	//Stopwatch destroyTimer;
	Vec2 deadPos;
};

class Game : public App::Scene
	//private Player
{
public:
	Game(const InitData& init);

	void update() override;

	void draw() const override;

private:
#ifdef _DEBUG
	void PrintDebug();
#endif // _DEBUG
	Visitor<void(void)> visitor;
	P2World world{ GRAVITY };
	Array<P2Body> chips;
	Array<P2Body> coins;
	//Array<P2Body> walkingEnemys;
	Array<WalkingEnemy> walkingEnemys;
	Array<FlyingEnemy> flyingEnemys;
	Array<CannonEnemy> cannonEnemys;
	Array<BulletEnemy> bulletEnemys;
	Grid<int8> enemyData =
	{
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	};

	Grid<int8> map;

	Camera2D camera{ Vec2{ 0, 0 } };

	Player player;
	void ControlPlayer();
	void ResponsePlayerLeftHit();
	void ResponsePlayerRightHit();
	void ResponsePlayerBottomHit();
	void ControlEnemys();
	[[maybe_unused]]
	void FirePakkun_oldCannonEnemy();
	bool isInputLeftDirection()
	{
		return (KeyA.pressed() or KeyLeft.pressed())
				or (KeyA.down() or KeyLeft.down());
	}
	bool isInputRightDirection()
	{
		return (KeyD.pressed() or KeyRight.pressed())
			or (KeyD.down() or KeyRight.down());

	}
	bool isInputDownDirection()
	{
		return (KeyS.pressed() or KeyDown.pressed());

	}
	bool isReleaseDownDirection()
	{
		return (KeyS.up() or KeyDown.up());
	}
	
};
