#include "stdafx.h"
#include "Game.h"
#include "LoadCSV.h"

Game::Game(const InitData& init) : IScene(init)
{
	//visitor.inspector[Accessor::first] = [&]() {return foo(); };
	//std::visit(visitor, visitor.inspector.at(visitor.stateMachine.front()));
	player.body() = world.createRect(P2Dynamic, Vec2(0, -100), RectF(CHIP_SIZE, CHIP_SIZE * 2), P2Material(1.0, 1.0, 0.2, 0.0));
	//weight = world.createRect(P2Dynamic, Vec2(-200, -1000),RectF(CHIP_SIZE, CHIP_SIZE * 2), P2Material(1.0,0,1.0,100.0));
	//weight.setSleepEnabled(false);
	player.body().setDamping(0.1);
	player.body().setFixedRotation(true);
	for (auto p : step(Size(enemyData.width(), enemyData.height()))) {
		if (enemyData[p.y][p.x] == 1)
			coins << world.createCircleSensor(P2Kinematic, Vec2{ p.x * CHIP_SIZE - 1280 / 2,p.y * CHIP_SIZE - 720 / 2 }, 10);
	}
	for (auto p : step(Size(enemyData.width(), enemyData.height()))) {
		if (enemyData[p.y][p.x] == 1)
			walkingEnemys << WalkingEnemy(world.createRect(P2Dynamic, Vec2{ p.x * CHIP_SIZE - 1280 / 2,p.y * CHIP_SIZE - 720 / 2 }, RectF(CHIP_SIZE), P2Material(10.0, 0.1, 0.0, 1.0)));
		if (enemyData[p.y][p.x] == 2)
			flyingEnemys << FlyingEnemy(world.createRect(P2Dynamic, Vec2{ p.x * CHIP_SIZE - 1280 / 2,p.y * CHIP_SIZE - 720 / 2 }, RectF(CHIP_SIZE), P2Material()));
		if (enemyData[p.y][p.x] == 3)
			cannonEnemys << CannonEnemy(world.createRect(P2Kinematic, Vec2{ p.x * CHIP_SIZE - 1280 / 2,p.y * CHIP_SIZE - 720 / 2 }, RectF(CHIP_SIZE), P2Material()));
	}
	const CSV mapData(U"example/csv/sample.csv");
	//mapData.columns(1);
	//mapData.rows();
	map.resize(mapData.columns(1), mapData.rows());
	for (auto y : step(mapData.rows()))
	{
		for (auto x : step(mapData.columns(1)))
		{
			int tile = GetTile(mapData, mapData.rows(), x, y);
			map[y][x] = tile;
		}
	}
	for (auto y : step(mapData.rows()))
	{
		for (auto x : step(mapData.columns(1)))
		{
			int tile = GetTile(mapData, mapData.rows(), x, y);
			map[y][x] = tile;
		}
	}
	//HACK: スライムブロックはPreviousVelocityを小さくすれば実装出来るかも？
	//ブロックの取得は敢えてMaterialsで密度を大きくしてGetMassで極端に重い物体を検知するとたぶん出来る
	Size size = { mapData.columns(1), mapData.rows() };//マップの大きさ｛ｘ、ｙ｝
	for (auto p : step(size)) {
		if (map[p.y][p.x] == 1)
			chips << world.createRect(P2Static, Vec2{ p.x * CHIP_SIZE - 1280 / 2,p.y * CHIP_SIZE - 720 / 2 }, RectF(CHIP_SIZE), P2Material());
		if (map[p.y][p.x] == 2)
			chips << world.createRect(P2Static, Vec2{ p.x * CHIP_SIZE - 1280 / 2,p.y * CHIP_SIZE - 720 / 2 }, RectF(CHIP_SIZE), P2Material());
		if (map[p.y][p.x] == 3)
			chips << world.createRect(P2Static, Vec2{ p.x * CHIP_SIZE - 1280 / 2,p.y * CHIP_SIZE - 720 / 2 }, RectF(CHIP_SIZE), P2Material());
		if (map[p.y][p.x] == 4)
			chips << world.createRect(P2Static, Vec2{ p.x * CHIP_SIZE - 1280 / 2,p.y * CHIP_SIZE - 720 / 2 }, RectF(CHIP_SIZE), P2Material());
	}
	for (auto&& enemy : walkingEnemys)
	{
		enemy.body().setFixedRotation(true);
		//enemy.body().addTriangle(Triangle{ CHIP_SIZE / 2, CHIP_SIZE, 20 ,180_deg }, P2Material(0.1, 0.1, 0.0, 1.0));
		enemy.body().addCircle(Circle{ Arg::center(CHIP_SIZE / 2,CHIP_SIZE), 10 }, P2Material(0.1, 0.1, 0.0, 1.0));
		enemy.body().addTriangle(Triangle{ 0,CHIP_SIZE / 2,40,270_deg }, P2Material(0.1, 0.1, 0.0, 1.0));
		enemy.body().addTriangle(Triangle{ CHIP_SIZE,CHIP_SIZE / 2,40,90_deg }, P2Material(0.1, 0.1, 0.0, 1.0));
		//enemy.body().addCircle(Circle{ Arg::center(0, CHIP_SIZE / 2), 10 }, P2Material(0.1, 0.1, 0.0, 1.0));
		//enemy.body().addCircle(Circle{ Arg::center(CHIP_SIZE, CHIP_SIZE / 2), 10 }, P2Material(0.1, 0.1, 0.0, 1.0));
	}
	for (auto&& enemy : flyingEnemys)
	{
		enemy.body().setFixedRotation(true);
		enemy.body().setGravityScale(0);
		enemy.body().addTriangle(Triangle{ CHIP_SIZE / 2,0,40, }, P2Material(1.0, 0.1, 0.0, 1.0));
		enemy.body().addTriangle(Triangle{ CHIP_SIZE / 2,CHIP_SIZE,40,180_deg }, P2Material(1.0, 0.1, 0.0, 1.0));
	}
	for (auto&& enemy : cannonEnemys)
	{
		enemy.body().setFixedRotation(true);
	}
	camera.setParameters(Camera2DParameters::NoControl());
}

void Game::update()
{
	for (ACCUMULATOR_SEC += Scene::DeltaTime(); STEP_SEC <= ACCUMULATOR_SEC; ACCUMULATOR_SEC -= STEP_SEC)
	{
		//auto& it = world.getCollisions();
		world.update(STEP_SEC);
		for (auto& it : world.getCollisions())
		{
			//PrintDebug();
			//底辺が接触したとき
			if (it.first.a == player.body().id()
				and it.second.normal() == Vec2(0, 1))
			{
				player.landingDelay().start();
				if (player.GetShouldRecordVelocity())
				{
					//反発した直後の速度を保存
					player.SetPreviousvelocity(player.body().getVelocity());
					player.SetShouldRecordVelocity(false);
				}
				player.body().setVelocity(Vec2(0, 0));
				player.body().setGravityScale(0);
				player.SetIsOnGround(true);
				for (auto&& enemy : walkingEnemys)
				{
					if (it.first.a == player.body().id()
						and it.first.b == enemy.GetBody().id())
					{
						//P2dynamicとの接触が起きた時のプレイヤーのリアクションは
						//P2staticもしくはP2kinematicとは別途書く必要がある（めんどくさい）
						enemy.body().release();

						player.landingDelay().reset();
						//playerのPreviousVelocityが上の方でy>0になってしまっているので跳ねない
						//YのVelocityを０に設定し直しておく
						player.body().setVelocity(Vec2(player.GetPreviousVelocity().x, 0));
						player.body().setGravityScale(1);
						player.body().applyLinearImpulse(Vec2(0, -500));
						player.SetIsJumpRestriction(true);
						player.SetIsOnGround(false);
						player.SetShouldRecordVelocity(true);
					}
				}
				for (auto&& enemy : flyingEnemys)
				{
					if (it.first.a == player.body().id()
						and it.first.b == enemy.GetBody().id())
					{
						enemy.body().release();
						if (not(KeySpace.down() or KeySpace.pressed()))
						{
							player.landingDelay().reset();
							player.body().setVelocity(Vec2(player.GetPreviousVelocity().x, 0));
							player.body().setGravityScale(1);
							player.body().applyLinearImpulse(Vec2(0, -500));
							player.SetIsJumpRestriction(true);
							player.SetIsOnGround(false);
							player.SetShouldRecordVelocity(true);
						}
					}
				}
			}

			//右辺が接触したとき
			if (it.first.a == player.body().id()
				and it.second.normal() == Vec2(1, 0)
				and (KeyD.pressed() or KeyRight.pressed()))
			{
				player.body().setVelocity(Vec2(0, 0));
				if (KeySpace.down())
				{
					player.body().applyLinearImpulse(Vec2(-player.param().wallKickPower.x, player.param().wallKickPower.y));
				}
			}

			if (it.first.a == player.body().id()
				and it.second.normal() == Vec2(1, 0)
				and (KeyD.up() or KeyRight.up()))
			{
				player.body().applyLinearImpulse(Vec2(-10, 0));
			}

			if (it.first.a == player.body().id()
				and it.second.normal() == Vec2(1, 0))
			{
				for (auto&& enemy : walkingEnemys)
				{
					if (it.first.a == player.body().id()
						and it.first.b == enemy.GetBody().id())
					{
						player.body().setVelocity(Vec2(0, 0));
						player.body().setGravityScale(1);
						player.body().applyLinearImpulse(Vec2(-300, -300));
						player.SetShouldRecordVelocity(true);
					}
				}
				for (auto&& enemy : flyingEnemys)
				{
					if (it.first.a == player.body().id()
						and it.first.b == enemy.GetBody().id())
					{
						player.body().setVelocity(Vec2(0, 0));
						player.body().setGravityScale(1);
						player.body().applyLinearImpulse(Vec2(-300, -300));
						player.SetShouldRecordVelocity(true);
					}
				}
			}
			//左辺が接触したとき
			if (it.first.a == player.body().id()
				and it.second.normal() == Vec2(-1, 0)
				and (KeyA.pressed() or KeyLeft.pressed()))
			{
				player.body().setVelocity(Vec2(0, 0));
				if (KeySpace.down())
				{
					player.body().applyLinearImpulse(player.param().wallKickPower);
				}
			}
			if (it.first.a == player.body().id()
				and it.second.normal() == Vec2(-1, 0)
				and (KeyA.up() or KeyLeft.up()))
			{
				player.body().applyLinearImpulse(Vec2(10, 0));
			}
			if (it.first.a == player.body().id()
				and it.second.normal() == Vec2(-1, 0))
			{
				for (auto&& enemy : walkingEnemys)
				{
					if (it.first.a == player.body().id()
						and it.first.b == enemy.GetBody().id())
					{
						player.body().setVelocity(Vec2(0, 0));
						player.body().setGravityScale(1);
						player.body().applyLinearImpulse(Vec2(300, -300));
						player.SetShouldRecordVelocity(true);
					}
				}
				for (auto&& enemy : flyingEnemys)
				{
					if (it.first.a == player.body().id()
						and it.first.b == enemy.GetBody().id())
					{
						player.body().setVelocity(Vec2(0, 0));
						player.body().setGravityScale(1);
						player.body().applyLinearImpulse(Vec2(300, -300));
						player.SetShouldRecordVelocity(true);
					}
				}
			}
			//上辺が接触したとき
			if (it.first.a == player.body().id()
				and it.second.normal() == Vec2(0, -1))
			{
				for (auto&& enemy : walkingEnemys)
				{
					if (it.first.a == player.body().id()
						and it.first.b == enemy.GetBody().id())
					{
					}
				}
				for (auto&& enemy : flyingEnemys)
				{
					if (it.first.a == player.body().id()
						and it.first.b == enemy.GetBody().id())
					{
					}
				}
			}
			//敵を主観としたときの当たり判定、AI行動の制御に使う
			for (auto&& enemy : walkingEnemys)
			{
				if (it.first.a == player.GetBody().id()
					and it.first.b == enemy.GetBody().id())
				{
					//プレイヤーと接触したときはなにもしない
				}
				else if ((it.first.a == enemy.GetBody().id() or
							it.first.b == enemy.GetBody().id())
						 and (it.second.normal() == Vec2(1, 0) or
							 it.second.normal() == Vec2(-1, 0)))
				{
					enemy.ToggleIsLookAtRight();
				}
			}
			for (auto&& enemy : flyingEnemys)
			{
				if (it.first.a == player.GetBody().id()
					and it.first.b == enemy.GetBody().id())
				{
					//プレイヤーと接触したときはなにもしない
				}
				else if ((it.first.a == enemy.GetBody().id() or
					it.first.b == enemy.GetBody().id())
						 and (it.second.normal() == Vec2(0, 1) or
							 it.second.normal() == Vec2(0, -1)))
				{
					enemy.ToggleIsLookAtDown();
					enemy.turnWatch().restart();
				}
				if (enemy.GetTurnWatch().ms() > 3000)
				{
					enemy.ToggleIsLookAtDown();
					enemy.turnWatch().restart();
				}
			}
			for (auto&& enemy : cannonEnemys)
			{
				//こいつは反転しない
				if (it.first.a == enemy.GetBody().id())
				{
				}
			}
			for (auto&& enemy : bulletEnemys)
			{
				//プレイヤーと接触したとき飛翔体が消滅する
				if (it.first.a == player.GetBody().id()
					and it.first.b == enemy.GetBody().id())
				{
					enemy.body().release();
				}
			}
		}
	}

	ControlPlayer();

	ControlEnemys();

	camera.update();
	camera.setCenter(Vec2(player.body().getPos().x, 0));
}

void Game::draw() const
{
	//TextureAsset(U"sampleBack").scaled(1.5,1.5).draw();
	const auto t = camera.createTransformer();
	player.GetBody().draw();
	/*TextureAsset(U"dummyPlayer")
		.scaled(0.5, 0.5)
		.draw(player.GetBody().getPos());*/
	for (const auto& chip : coins)
	{
		chip.draw(HSV{ chip.id() * 10.0 });
	}
	for (const auto& enemy : walkingEnemys)
	{
		enemy.GetBody().draw(HSV{ enemy.GetBody().id() * 10.0 });
	}
	for (const auto& enemy : flyingEnemys)
	{
		enemy.GetBody().draw(HSV{ enemy.GetBody().id() * 10.0 });
	}
	for (const auto& enemy : cannonEnemys)
	{
		enemy.GetBody().draw(HSV{ enemy.GetBody().id() * 10.0 });
	}

	for (const auto& enemy : bulletEnemys)
	{
		enemy.GetBody().draw(HSV{ enemy.GetBody().id() * 10.0 });
	}

	for (const auto& chip : chips)
	{
		chip.draw(HSV{ chip.id() * 10.0 });
		int id = chip.id();
		PutText(String(ToString(id)), chip.getPos() + Vec2(CHIP_SIZE / 2, CHIP_SIZE / 2));
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
}

void Game::ControlPlayer()
{
	if (player.landingDelay().ms() > 500)
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
		player.landingDelay().reset();
		player.body().setVelocity(player.GetPreviousVelocity());
		player.SetShouldRecordVelocity(true);
		player.SetIsOnGround(false);
		player.SetIsJumpRestriction(true);
		player.body().setGravityScale(1);
	}
	if (player.GetIsOnGround())
	{
		if ((KeySpace.pressed() or KeySpace.down())
			and KeyA.pressed())
		{
			player.landingDelay().reset();
			player.body().setVelocity(player.GetPreviousVelocity());
			player.body().setGravityScale(1);
			player.body().applyLinearImpulse(Vec2(-player.param().floorKickPower, player.param().jumpPower / 4));
			player.SetIsJumpRestriction(true);
			player.SetIsOnGround(false);
			player.SetShouldRecordVelocity(true);
		}
		if ((KeySpace.pressed() or KeySpace.down())
			and KeyD.pressed())
		{
			player.landingDelay().reset();
			player.body().setVelocity(player.GetPreviousVelocity());
			player.body().setGravityScale(1);
			player.body().applyLinearImpulse(Vec2(player.param().floorKickPower, player.param().jumpPower / 4));
			player.SetIsJumpRestriction(true);
			player.SetIsOnGround(false);
			player.SetShouldRecordVelocity(true);
		}
		if ((KeySpace.pressed() or KeySpace.down())
			and not(KeyA.pressed() or KeyD.pressed()))
		{
			player.landingDelay().reset();
			player.body().setVelocity(player.GetPreviousVelocity());
			player.body().setGravityScale(1);
			player.body().applyLinearImpulse(Vec2(0, player.param().jumpPower));
			player.SetIsJumpRestriction(true);
			player.SetIsOnGround(false);
			player.SetShouldRecordVelocity(true);
		}
	}
	if (KeySpace.up())
	{
		player.SetIsJumpRestriction(false);
	}
	if (KeyS.pressed())
	{
		player.landingDelay().reset();
		player.body().setDamping(0.95);
	}
	/*if (KeyS.pressed()
		and (KeyA.pressed() or KeyD.pressed()))
	{
		player.body().setDamping(0.3);
	}*/
	if (KeyS.up())
	{
		player.body().setDamping(0.1);
	}
	if (KeyS.up()
		and player.GetIsOnGround())
	{
		player.landingDelay().reset();
		player.body().setGravityScale(1);
		player.body().applyLinearImpulse(Vec2(0, player.param().jumpPower * 10));
		player.SetIsJumpRestriction(true);
		player.SetIsOnGround(false);
		player.body().setDamping(0.1);
		player.SetShouldRecordVelocity(true);
	}

	if ((KeyA.pressed() or KeyLeft.pressed())
		and not player.GetIsOnGround())
	{
		player.body().applyForce(Vec2(-player.param().accelerationX * Scene::DeltaTime(), 0));
	}
	if ((KeyD.pressed() or KeyRight.pressed())
		and not player.GetIsOnGround())
	{
		player.body().applyForce(Vec2(player.param().accelerationX * Scene::DeltaTime(), 0));
	}

	double vel = Clamp(player.body().getVelocity().x, -player.param().maxVelocity.x, player.param().maxVelocity.x);
	player.body().setVelocity(Vec2(vel, player.body().getVelocity().y));
}

void Game::ControlEnemys()
{
	for (auto&& enemy : walkingEnemys)
	{
		if (enemy.GetIsLookAtRight())
		{
			enemy.body().applyForce(Vec2(1000000 * Scene::DeltaTime(), 0));
		}
		else
		{
			enemy.body().applyForce(Vec2(-(1000000 * Scene::DeltaTime()), 0));
		}
		Vec2 clampVel{};
		clampVel.x = Clamp(enemy.body().getVelocity().x, -100.0, 100.0);
		clampVel.y = Clamp(enemy.body().getVelocity().y, -10000.0, 10000.0);
		enemy.body().setVelocity(clampVel);
	}

	for (auto&& enemy : flyingEnemys)
	{
		if (enemy.GetIsLookAtDown())
		{
			enemy.body().applyForce(Vec2(0, 50000 * Scene::DeltaTime()));
		}
		else
		{
			enemy.body().applyForce(Vec2(0, -50000 * Scene::DeltaTime()));
		}
		Vec2 clampVel{};
		clampVel.x = 0;
		clampVel.y = Clamp(enemy.body().getVelocity().y, -200.0, 200.0);
		enemy.body().setVelocity(clampVel);
	}
	for (auto&& enemy : cannonEnemys)
	{
		if (enemy.GetShotInterval().ms() > 3000)
		{
			BulletEnemy bullet = BulletEnemy(world.createCircleSensor(P2Dynamic, enemy.GetBody().getPos() + Vec2(0, CHIP_SIZE / 2), 10)
										, Vec2(-100, 0));
			bullet.body().setGravityScale(0);
			bulletEnemys << bullet;
			enemy.shotInterval().restart();
		}

	}
	for (auto&& bullet : bulletEnemys)
	{
		//固定値で動かす
		bullet.body().moveBy(bullet.GetVelocity() * Scene::DeltaTime());

	}
	for (auto&& coin : coins)
	{
		for (auto&& it : world.getCollisions())
		{
			if (it.first.a == player.GetBody().id()
				and it.first.b == coin.id())
			{
				coin.release();
			}
		}
	}
}

void Game::FirePakkun_oldCannonEnemy()
{
	for (auto&& enemy : cannonEnemys)
	{
		if (enemy.GetShotInterval().ms() > 3000)
		{
			bulletEnemys << BulletEnemy(world.createCircleSensor(P2Kinematic, enemy.GetBody().getPos(), 10),
										Normalize((player.GetBody().getPos() + Vec2(CHIP_SIZE / 2, CHIP_SIZE)) - enemy.GetBody().getPos()) * 5);

			enemy.shotInterval().restart();
		}

	}
}
