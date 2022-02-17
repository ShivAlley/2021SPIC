#include "stdafx.h"
#include "Game.h"


Game::Game(const InitData& init):IScene(init)
{
	//visitor.inspector[Accessor::first] = [&]() {return foo(); };
	//std::visit(visitor, visitor.inspector.at(visitor.stateMachine.front()));
	player.LVRbody() = world.createRect(P2Dynamic, Vec2(0, -100), RectF(CHIP_SIZE, CHIP_SIZE * 2), P2Material(1.0,1.0,0.2,0.0));
	//player.LVRbody().addRect(RectF(30, 100));

	player.LVRbody().addTriangle(Triangle(32+16,0,42+16,32,32+16,64), P2Material(1.0, 1.0, 0.2, 0.0));
	player.LVRbody().addTriangle(Triangle(16,0,4,32,16,64), P2Material(1.0, 1.0, 0.2, 0.0));

	player.LVRbody().setDamping(0.1);
	player.LVRbody().setFixedRotation(true);
	//RectF rr(Arg::center(CHIP_SIZE,CHIP_SIZE * 2), CHIP_SIZE, 32 / 2);
	//player.LVRbody().addRect(rr);
	Size size = { map.width(), map.height() };//マップの大きさ｛ｘ、ｙ｝
	for (auto p : step(size)) {
		if (map[p.y][p.x] == 1)
			chips << world.createRect(P2Static, Vec2{ p.x * 64 - 1280 / 2,p.y * 64 - 720 / 2 }, RectF(64, 64, 64, 64), P2Material());
	}
	for (auto p : step(Size(enemy.width(),enemy.height()))) {
		if (enemy[p.y][p.x] == 1)
			enemys << world.createRect(P2Dynamic, Vec2{ p.x * 64 - 1280 / 2,p.y * 64 - 720 / 2 }, RectF(64, 64, 64, 64), P2Material());
	}
	camera.setParameters(Camera2DParameters::NoControl());
}




void Game::update()
{
	
	for (accumulatorSec += Scene::DeltaTime(); stepSec <= accumulatorSec; accumulatorSec -= stepSec)
	{
		world.update(stepSec);

		PrintDebug();
		for (auto& it : world.getCollisions())
		{
			//底辺が接触したとき
			if (it.second.normal() == Vec2(0, 1)
				and (KeySpace.pressed() or KeySpace.down())
				and KeyA.pressed()
				and not player.isJumpRestriction())
			{
				player.LVRbody().applyLinearImpulse(Vec2(-player.param().KICK_POWER_X , player.param().JUMP_POWER / 4));
				player.LVRisJumpRestriction() = true;
			}
			if (it.second.normal() == Vec2(0, 1)
				and (KeySpace.pressed() or KeySpace.down())
				and KeyD.pressed()
				and not player.isJumpRestriction())
			{
				player.LVRbody().applyLinearImpulse(Vec2(player.param().KICK_POWER_X, player.param().JUMP_POWER / 4));
				player.LVRisJumpRestriction() = true;
			}
			if (it.second.normal() == Vec2(0, 1)
				and (KeySpace.pressed() or KeySpace.down())
				and not(KeyA.pressed() or KeyD.pressed())
				and not player.isJumpRestriction())
			{
				player.LVRbody().applyLinearImpulse(Vec2(0, player.param().JUMP_POWER));
				player.LVRisJumpRestriction() = true;
			}
			//右辺が接触したとき
			if (it.second.normal() == Vec2(1, 0)
				and (KeyD.pressed() or KeyRight.pressed()))
			{
				player.LVRbody().setVelocity(Vec2(0, 0));
				if (KeySpace.down())
				{
					player.LVRbody().applyLinearImpulse(Vec2(-500, -500));
				}
			}
			if (it.second.normal() == Vec2(1, 0)
				and (KeyD.up() or KeyRight.up()))
			{
				player.LVRbody().applyLinearImpulse(Vec2(-10, 0));
			}
			//左辺が接触したとき
			if (it.second.normal() == Vec2(-1, 0)
				and (KeyA.pressed() or KeyLeft.pressed()))
			{
				player.LVRbody().setVelocity(Vec2(0, 0));
				if (KeySpace.down())
				{
					player.LVRbody().applyLinearImpulse(Vec2(500, -500));
				}
			}
			if (it.second.normal() == Vec2(-1, 0)
				and (KeyA.up() or KeyLeft.up()))
			{
				player.LVRbody().applyLinearImpulse(Vec2(10, 0));
			}
		}
		
	}
	if (KeySpace.up())
	{
		player.LVRisJumpRestriction() = false;
	}

	if (KeyA.pressed() or KeyLeft.pressed())
	{
		player.LVRbody().applyForce(Vec2(-player.param().ACCELERATION_X * Scene::DeltaTime(), 0));
	}
	if (KeyD.pressed() or KeyRight.pressed())
	{
		player.LVRbody().applyForce(Vec2(player.param().ACCELERATION_X * Scene::DeltaTime(), 0));
	}
	double vel = Clamp(player.body().getVelocity().x, -player.param().MAX_VELOCITY_X, player.param().MAX_VELOCITY_X);
	player.LVRbody().setVelocity(Vec2(vel, player.body().getVelocity().y));

	camera.update();
	camera.setCenter(Vec2(player.body().getPos().x, 200));
	//camera.setScale(1);
	
	//PrintDebug();
}

void Game::draw() const
{
	//TextureAsset(U"sampleBack").scaled(1.5,1.5).draw();
	const auto t = camera.createTransformer();
	player.body().draw();
	TextureAsset(U"dummyPlayer")
		.scaled(0.5,0.5)
		.draw(player.body().getPos());
	for (const auto& chip : chips)
	{
		chip.draw(HSV{ chip.id() * 10.0 });
	}
	for (const auto& enemy : enemys)
	{
		enemy.draw(HSV{ enemy.id() * 10.0 });
	}	
}

void Game::PrintDebug()
{
	ClearPrint();
	for (auto& it : world.getCollisions())
	{
		Print << it.first.a;
		Print << it.first.b;
		Print << it.second.normal();
	}
	Print << player.body().getInertia();
	Print << player.body().getVelocity();
}
