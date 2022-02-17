#include "stdafx.h"
#include "Game.h"


Game::Game(const InitData& init):IScene(init)
{
	//visitor.inspector[Accessor::first] = [&]() {return foo(); };
	//std::visit(visitor, visitor.inspector.at(visitor.stateMachine.front()));
	player.m_body = world.createRect(P2Dynamic, Vec2(0, -100), RectF(CHIP_SIZE, CHIP_SIZE * 2), P2Material(1.0,1.0,0.2,0.0));
	weight = world.createRect(P2Dynamic, Vec2(-200, -1000),RectF(CHIP_SIZE, CHIP_SIZE * 2), P2Material(1.0,0,1.0,100.0));
	player.m_body.setDamping(0.1);
	player.m_body.setFixedRotation(true);
	Size size = { map.width(), map.height() };//マップの大きさ｛ｘ、ｙ｝
	for (auto p : step(size)) {
		if (map[p.y][p.x] == 1)
			chips << world.createRect(P2Static, Vec2{ p.x * CHIP_SIZE - 1280 / 2,p.y * CHIP_SIZE - 720 / 2 }, RectF(CHIP_SIZE), P2Material());
	}
	for (auto p : step(Size(enemy.width(),enemy.height()))) {
		if (enemy[p.y][p.x] == 1)
			enemys << world.createRect(P2Dynamic, Vec2{ p.x * CHIP_SIZE - 1280 / 2,p.y * CHIP_SIZE - 720 / 2 }, RectF(CHIP_SIZE), P2Material());
	}
	camera.setParameters(Camera2DParameters::NoControl());
}




void Game::update()
{
	
	for (accumulatorSec += Scene::DeltaTime(); stepSec <= accumulatorSec; accumulatorSec -= stepSec)
	{
		world.update(stepSec);
		for (auto& it : world.getCollisions())
		{
			PrintDebug();
			//底辺が接触したとき
			if (it.first.a == player.m_body.id()
				and it.second.normal() == Vec2(0, 1))
			{
				player.m_landingDelay.start();
				if (player.shouldRecordVelocity())
				{
					//反発した直後の速度を保存
					player.SetPreviousvelocity(player.m_body.getVelocity());
					player.SetShouldRecordVelocity(false);
				}
				player.m_body.setVelocity(Vec2(0, 0));
				player.m_body.setGravityScale(0);
				player.SetIsOnGround(true);
				
			}
			if (player.m_landingDelay.ms() > 500)
			{
				if (KeyA.pressed()
					and player.GetPreviousVelocity().x > 0)
				{
					player.SetPreviousvelocity(Vec2(0, player.GetPreviousVelocity().y));
				}
				if (KeyD.pressed()
					and player.GetPreviousVelocity().x < 0)
				{
					player.SetPreviousvelocity(Vec2(0, player.GetPreviousVelocity().y));
				}
				player.m_landingDelay.reset();
				player.m_body.setVelocity(player.GetPreviousVelocity());
				player.SetShouldRecordVelocity(true);
				player.SetIsOnGround(false);
				player.SetIsJumpRestriction(true);
				player.m_body.setGravityScale(1);
				
			}
			else 
			{
				if ((KeySpace.pressed() or KeySpace.down())
					and KeyA.pressed()
					and player.GetIsOnGround())
				{
					player.m_landingDelay.reset();
					player.m_body.setVelocity(player.GetPreviousVelocity());
					player.m_body.setGravityScale(1);
					player.m_body.applyLinearImpulse(Vec2(-player.m_param.floorKickPower, player.m_param.jumpPower / 4));
					player.SetIsJumpRestriction(true);
					player.SetIsOnGround(false);
					player.SetShouldRecordVelocity(true);
				}
				if ((KeySpace.pressed() or KeySpace.down())
					and KeyD.pressed()
					and player.GetIsOnGround())
				{
					player.m_landingDelay.reset();
					player.m_body.setVelocity(player.GetPreviousVelocity());
					player.m_body.setGravityScale(1);
					player.m_body.applyLinearImpulse(Vec2(player.m_param.floorKickPower, player.m_param.jumpPower / 4));
					player.SetIsJumpRestriction(true);
					player.SetIsOnGround(false);
					player.SetShouldRecordVelocity(true);
				}
				if ((KeySpace.pressed() or KeySpace.down())
					and not(KeyA.pressed() or KeyD.pressed())
					and player.GetIsOnGround())
				{
					player.m_landingDelay.reset();
					player.m_body.setVelocity(player.GetPreviousVelocity());
					player.m_body.setGravityScale(1);
					player.m_body.applyLinearImpulse(Vec2(0, player.m_param.jumpPower));
					player.SetIsJumpRestriction(true);
					player.SetIsOnGround(false);
					player.SetShouldRecordVelocity(true);
				}
			}
			if (KeyS.pressed())
			{
				player.m_landingDelay.reset();
				player.m_body.setDamping(0.99);
			}
			if (KeyS.up()
				and player.GetIsOnGround())
			{
				player.m_landingDelay.reset();
				//player.body.setVelocity(player.GetPreviousVelocity());
				player.m_body.setGravityScale(1);
				player.m_body.applyLinearImpulse(Vec2(0, player.m_param.jumpPower * 10));
				player.SetIsJumpRestriction(true);
				player.SetIsOnGround(false);
				player.m_body.setDamping(0.1);
				player.SetShouldRecordVelocity(true);
			}
			//右辺が接触したとき
			if (it.first.a == player.m_body.id()
				and it.second.normal() == Vec2(1, 0)
				and (KeyD.pressed() or KeyRight.pressed()))
			{
				player.m_body.setVelocity(Vec2(0, 0));
				if (KeySpace.down())
				{
					player.m_body.applyLinearImpulse(Vec2(-player.m_param.wallKickPower.x,player.m_param.wallKickPower.y));
				}
			}

			if (it.first.a == player.m_body.id()
				and it.second.normal() == Vec2(1, 0)
				and (KeyD.up() or KeyRight.up()))
			{
				player.m_body.applyLinearImpulse(Vec2(-10, 0));
			}
			//左辺が接触したとき
			if (it.first.a == player.m_body.id()
				and it.second.normal() == Vec2(-1, 0)
				and (KeyA.pressed() or KeyLeft.pressed()))
			{
				player.m_body.setVelocity(Vec2(0, 0));
				if (KeySpace.down())
				{
					player.m_body.applyLinearImpulse(player.m_param.wallKickPower);
				}
			}
			if (it.first.a == player.m_body.id()
				and it.second.normal() == Vec2(-1, 0)
				and (KeyA.up() or KeyLeft.up()))
			{
				player.m_body.applyLinearImpulse(Vec2(10, 0));
			}
		}
		
	}

	if (KeySpace.up())
	{
		player.SetIsJumpRestriction(false);
	}
	if (KeyS.up())
	{
		player.m_body.setDamping(0.1);
	}
	if (player.m_landingDelay.ms() > 500)
	{
		if (KeyA.pressed()
			and player.GetPreviousVelocity().x > 0)
		{
			player.SetPreviousvelocity(Vec2(0, player.GetPreviousVelocity().y));
		}
		if (KeyD.pressed()
			and player.GetPreviousVelocity().x < 0)
		{
			player.SetPreviousvelocity(Vec2(0, player.GetPreviousVelocity().y));
		}
		player.m_landingDelay.reset();
		player.m_body.setVelocity(player.GetPreviousVelocity());
		player.SetShouldRecordVelocity(true);
		player.SetIsOnGround(false);
		player.SetIsJumpRestriction(true);
		player.m_body.setGravityScale(1);

	}

	if ((KeyA.pressed() or KeyLeft.pressed())
		and not player.GetIsOnGround())
	{
		player.m_body.applyForce(Vec2(-player.m_param.accelerationX * Scene::DeltaTime(), 0));
	}
	if ((KeyD.pressed() or KeyRight.pressed())
		and not player.GetIsOnGround())
	{
		player.m_body.applyForce(Vec2(player.m_param.accelerationX * Scene::DeltaTime(), 0));
	}
	double vel = Clamp(player.m_body.getVelocity().x, -player.m_param.maxVelocity.x, player.m_param.maxVelocity.x);
	player.m_body.setVelocity(Vec2(vel, player.m_body.getVelocity().y));

	//enemys.remove_if([](P2Body body) {return body.getPos().y > 1000; });

	camera.update();
	camera.setCenter(Vec2(player.m_body.getPos().x, 200));
}

void Game::draw() const
{
	//TextureAsset(U"sampleBack").scaled(1.5,1.5).draw();
	const auto t = camera.createTransformer();
	player.m_body.draw();
	TextureAsset(U"dummyPlayer")
		.scaled(0.5,0.5)
		.draw(player.m_body.getPos());
	for (const auto& chip : chips)
	{
		chip.draw(HSV{ chip.id() * 10.0 });
		
	}
	for (const auto& it : enemys)
	{
		it.draw(HSV{ it.id() * 10.0 });
	}
	//test.draw(Palette::Pink);
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
	Print << player.m_body.getVelocity();
	Print << player.GetDeltaVelocity();
	Print << player.GetPreviousVelocity();
	for (const auto& it : enemys)
	{
		Print << it.getPos();
	}
}


