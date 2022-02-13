#include "stdafx.h"
#include "Game.h"


Game::Game(const InitData& init):IScene(init)
{
	//visitor.inspector[Accessor::first] = [&]() {return foo(); };
	//std::visit(visitor, visitor.inspector.at(visitor.stateMachine.front()));
	player.LVRbody() = world.createRect(P2Dynamic, Vec2(0, -100), RectF(Arg::center(CHIP_SIZE,CHIP_SIZE),CHIP_SIZE, CHIP_SIZE * 2), P2Material(1.0,1.0,0.2,0.0));
	//player.LVRbody().addRect(RectF(30, 100));
	player.LVRbody().setDamping(0.1);
	player.LVRbody().setFixedRotation(true);
	//RectF rr(Arg::center(CHIP_SIZE,CHIP_SIZE * 2), CHIP_SIZE, 32 / 2);
	//player.LVRbody().addRect(rr);
	Size size = { map.width(), map.height() };//マップの大きさ｛ｘ、ｙ｝
	for (auto p : step(size)) {
		if (map[p.y][p.x] == 1)
			chips << world.createRect(P2Static, Vec2{ p.x * 32 - 1280 / 2,p.y * 32 - 720 / 2 }, RectF(32, 32, 32, 32), P2Material());
	}
	for (auto p : step(Size(enemy.width(),enemy.height()))) {
		if (enemy[p.y][p.x] == 1)
			enemys << world.createRect(P2Dynamic, Vec2{ p.x * 32 - 1280 / 2,p.y * 32 - 720 / 2 }, RectF(32, 32, 32, 32), P2Material());
	}
	camera.setParameters(Camera2DParameters::NoControl());
}




void Game::update()
{
	//ClearPrint();
	for (accumulatorSec += Scene::DeltaTime(); stepSec <= accumulatorSec; accumulatorSec -= stepSec)
	{
		world.update(stepSec);

		PrintDebug();
		for (auto& it : world.getCollisions())
		{
			if (it.second.normal() == Vec2(0, 1)
				and (KeySpace.pressed() or KeySpace.down())
				and not player.isJumpRestriction())
			{
				player.LVRbody().applyLinearImpulse(Vec2(0, player.param().JUMP_POWER));
				player.LVRisJumpRestriction() = true;
			}
			if (it.second.normal() == Vec2(1, 0)
				and (KeyD.pressed() or KeyRight.pressed()))
			{
				player.LVRbody().setVelocity(Vec2(0, 0));
				if (KeySpace.down())
				{
					player.LVRbody().applyLinearImpulse(Vec2(-500, -100));
				}
			}
			if (it.second.normal() == Vec2(1, 0)
				and (KeyD.up() or KeyRight.up()))
			{
				player.LVRbody().applyLinearImpulse(Vec2(-1000, 0));
			}
			if (it.second.normal() == Vec2(-1, 0)
				and (KeyA.pressed() or KeyLeft.pressed()))
			{
				player.LVRbody().setVelocity(Vec2(0, 0));
				if (KeySpace.down())
				{
					player.LVRbody().applyLinearImpulse(Vec2(500, -100));
				}
			}
			if (it.second.normal() == Vec2(-1, 0)
				and (KeyA.up() or KeyLeft.up()))
			{
				player.LVRbody().applyLinearImpulse(Vec2(1000, 0));
			}
		}
		
	}
	if (KeySpace.up())
	{
		player.LVRisJumpRestriction() = false;
	}

	if (KeyA.pressed() or KeyLeft.pressed())
	{
		player.LVRbody().applyForce(Vec2(-player.param().ACCELERATION_X, 0));
	}
	if (KeyD.pressed() or KeyRight.pressed())
	{
		player.LVRbody().applyForce(Vec2(player.param().ACCELERATION_X, 0));
	}
	double vel = Clamp(player.body().getVelocity().x, -player.param().MAX_VELOCITY_X, player.param().MAX_VELOCITY_X);
	player.LVRbody().setVelocity(Vec2(vel, player.body().getVelocity().y));

	camera.update();
	camera.setCenter(player.body().getPos());
	//camera.setScale(0.5);
	
	//PrintDebug();
}

void Game::draw() const
{
	//TextureAsset(U"sampleBack").scaled(1.5,1.5).draw();
	const auto t = camera.createTransformer();
	player.body().draw();
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
	for (auto& it : world.getCollisions())
	{
		Print << it.first.a;
		Print << it.first.b;
		Print << it.second.normal();
	}
}
