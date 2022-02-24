#include "stdafx.h"
#include "Game.h"
#include "LoadCSV.h"

Game::Game(const InitData& init) : IScene(init)
{
	//visitor.inspector[Accessor::first] = [&]() {return foo(); };
	//std::visit(visitor, visitor.inspector.at(visitor.stateMachine.front()));
	//TopLeftをずらして指定して描画の時に地に足がつくようにする
	//player.body() = world.createRect(P2Dynamic, Vec2(500, -100), RectF(Arg::topLeft(0,32),BODY_SIZE, BODY_SIZE * 2), P2Material(1.0, 1.0, 0.2, 0.0));
	player.body() = world.createRect(P2Dynamic, Vec2(500, -100), RectF(CHIP_SIZE, CHIP_SIZE * 2), P2Material(1.0, 1.0, 0.2, 0.0));
	player.body().setDamping(0.1);
	player.body().setFixedRotation(true);
	for (auto p : step(Size(enemyData.width(), enemyData.height()))) {
		if (enemyData[p.y][p.x] == 1)
			coins << world.createCircleSensor(P2Kinematic, Vec2{ p.x * CHIP_SIZE ,p.y * CHIP_SIZE  }, 10);
		//TODO:仮のコイン
	}
	for (auto p : step(Size(enemyData.width(), enemyData.height()))) {
		if (enemyData[p.y][p.x] == 1)
			walkingEnemys << WalkingEnemy(world.createRect(P2Dynamic, Vec2{ p.x * CHIP_SIZE ,p.y * CHIP_SIZE  }, RectF(CHIP_SIZE), P2Material(10.0, 0.1, 0.0, 1.0)));
		if (enemyData[p.y][p.x] == 2)
			flyingEnemys << FlyingEnemy(world.createRect(P2Dynamic, Vec2{ p.x * CHIP_SIZE ,p.y * CHIP_SIZE  }, RectF(CHIP_SIZE), P2Material()));
		if (enemyData[p.y][p.x] == 3)
			cannonEnemys << CannonEnemy(world.createRect(P2Kinematic, Vec2{ p.x * CHIP_SIZE ,p.y * CHIP_SIZE  }, RectF(CHIP_SIZE), P2Material()));
	}
	const CSV mapData(U"example/csv/sample.csv");
	map.resize(mapData.columns(1), mapData.rows());
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
			chips << world.createRect(P2Static, Vec2{ p.x * CHIP_SIZE ,p.y * CHIP_SIZE  }, RectF(CHIP_SIZE), P2Material());
		if (map[p.y][p.x] == 2)
			chips << world.createRect(P2Static, Vec2{ p.x * CHIP_SIZE ,p.y * CHIP_SIZE  }, RectF(CHIP_SIZE), P2Material());
		if (map[p.y][p.x] == 3)
			chips << world.createRect(P2Static, Vec2{ p.x * CHIP_SIZE ,p.y * CHIP_SIZE  }, RectF(CHIP_SIZE), P2Material());
		if (map[p.y][p.x] == 4)
			chips << world.createRect(P2Static, Vec2{ p.x * CHIP_SIZE ,p.y * CHIP_SIZE  }, RectF(CHIP_SIZE), P2Material());
	}

	for (auto&& enemy : walkingEnemys)
	{
		enemy.body().setFixedRotation(true);
		//enemy.body().addTriangle(Triangle{ CHIP_SIZE / 2, CHIP_SIZE, 20 ,180_deg }, P2Material(0.1, 0.1, 0.0, 1.0));
		enemy.body().addCircle(Circle{ Arg::center(CHIP_SIZE / 2,CHIP_SIZE), 10 }, P2Material(0.1, 0.1, 0.0, 1.0));
		enemy.body().addTriangle(Triangle{ 0,CHIP_SIZE / 2,40,270_deg }, P2Material(0.1, 0.1, 0.0, 1.0));
		enemy.body().addTriangle(Triangle{ CHIP_SIZE,CHIP_SIZE / 2,40,90_deg }, P2Material(0.1, 0.1, 0.0, 1.0));
		//enemy.body().addCircleSensor(Circle{ Arg::center(-4,CHIP_SIZE + 10),5 });
		//enemy.body().addCircleSensor(Circle{ Arg::center(CHIP_SIZE + 4,CHIP_SIZE + 10),5 });
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
	camera.setCenter(player.GetBody().getPos());
	camera.setParameters(Camera2DParameters::NoControl());
}

void Game::update()
{
	for (ACCUMULATOR_SEC += Scene::DeltaTime(); STEP_SEC <= ACCUMULATOR_SEC; ACCUMULATOR_SEC -= STEP_SEC)
	{
		//PrintDebug();
		world.update(STEP_SEC);
		for (auto&& it : world.getCollisions())
		{
			//プレイヤーの主観での衝突判定
			// 対応する辺ごとに敵のRangeを回しているよ　かわいいね（クソコード）
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
				player.SetIsAttachWall(false);
				for (auto&& enemy : walkingEnemys)
				{
					if (it.first.a == player.body().id()
						and it.first.b == enemy.GetBody().id())
					{
						//P2dynamicとの接触が起きた時のプレイヤーのリアクションは
						//P2staticもしくはP2kinematicとは別途書く必要がある（めんどくさい）
						enemy.body().release();

						ResponsePlayerBottomHit();
					}
				}
				for (auto&& enemy : flyingEnemys)
				{
					if (it.first.a == player.body().id()
						and it.first.b == enemy.GetBody().id())
					{
						enemy.body().release();
						ResponsePlayerBottomHit();
					}
				}
			}
			
			
			//右辺が接触したとき
			if (it.first.a == player.body().id()
				and it.second.normal() == Vec2(1, 0)
				and isInputRightDirection()
				and not player.GetIsInvincible())
			{
				player.SetIsAttachWall(true);
				player.body().setVelocity(Vec2(0, 0));
				if (KeySpace.down())
				{
					player.SetIsAttachWall(false);
					player.body().applyLinearImpulse(Vec2(-player.GetParam().wallKickPower.x, player.GetParam().wallKickPower.y));
				}
			}

			if (it.first.a == player.body().id()
				and it.second.normal() == Vec2(1, 0)
				and (KeyD.up() or KeyRight.up()))
			{
				player.SetIsAttachWall(false);
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
						ResponsePlayerRightHit();
					}
				}
				for (auto&& enemy : flyingEnemys)
				{
					if (it.first.a == player.body().id()
						and it.first.b == enemy.GetBody().id())
					{
						ResponsePlayerRightHit();

					}
				}
			}
			//左辺が接触したとき
			if (it.first.a == player.body().id()
				and it.second.normal() == Vec2(-1, 0)
				and isInputLeftDirection()
				and not player.GetIsInvincible())
			{
				player.SetIsAttachWall(true);
				player.body().setVelocity(Vec2(0, 0));
				if (KeySpace.down())
				{
					player.SetIsAttachWall(false);
					player.body().applyLinearImpulse(player.GetParam().wallKickPower);
				}
			}
			if (it.first.a == player.body().id()
				and it.second.normal() == Vec2(-1, 0)
				and (KeyA.up() or KeyLeft.up()))
			{
				player.SetIsAttachWall(false);
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
						ResponsePlayerLeftHit();

					}
				}
				for (auto&& enemy : flyingEnemys)
				{
					if (it.first.a == player.body().id()
						and it.first.b == enemy.GetBody().id())
					{
						ResponsePlayerLeftHit();

					}
				}
			}
			//上辺が接触したとき
			if (it.first.a == player.body().id()
				and it.second.normal() == Vec2(0, -1)
				and not player.GetIsInvincible())
			{
				for (auto&& enemy : walkingEnemys)
				{
					if (it.first.a == player.body().id()
						and it.first.b == enemy.GetBody().id())
					{
						//プレイヤーの中心座標が敵より右にあったら
						//反対側（左方向へ）ノックバックを受ける
						//つまり右からぶつかっている時
						if (player.GetBody().getPos().x + CHIP_SIZE / 2 > enemy.GetBody().getPos().x + CHIP_SIZE / 2)
						{
							ResponsePlayerRightHit();
						}
						else //左だったら逆
						{
							ResponsePlayerLeftHit();
						}
					}
				}
				for (auto&& enemy : flyingEnemys)
				{
					if (it.first.a == player.body().id()
						and it.first.b == enemy.GetBody().id())
					{
						//プレイヤーの中心座標が敵より右にあったら
						//反対側（左方向へ）ノックバックを受ける
						//つまり右からぶつかっている時
						if (player.GetBody().getPos().x + CHIP_SIZE / 2 > enemy.GetBody().getPos().x + CHIP_SIZE / 2)
						{
							ResponsePlayerRightHit();

						}
						else //左だったら逆
						{
							ResponsePlayerLeftHit();

						}
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
							 it.second.normal() == Vec2(-1, 0))
						and enemy.turnCooldown().ms() > 100)
				{
					enemy.ToggleIsLookAtRight();
					enemy.turnCooldown().restart();
				}
				//マップデータを参照して何もない空間を検知すると反転する
				//高速回転バグの対策として反転にはクールタイムを用意している
				//原始的な対策であるため外れ値には対応できない
				Vector2D<int32> vec = enemy.GetBody().getPos() + Vector2D<int32>(0,CHIP_SIZE + CHIP_SIZE / 2);
				if (map[vec.y / CHIP_SIZE][vec.x / CHIP_SIZE] == 0
					and enemy.turnCooldown().ms() > 500)
				{
					enemy.ToggleIsLookAtRight();
					enemy.turnCooldown().restart();
				}
			}
			for (auto&& enemy : flyingEnemys)
			{
				if (it.first.a == player.GetBody().id()
					and it.first.b == enemy.GetBody().id())
				{
					//プレイヤーと接触したときはなにもしない
				}
				//壁にぶつかるか5秒経過すると反転する
				else if (it.first.a == enemy.GetBody().id()
						 and (it.second.normal() == Vec2(0, 1) or
							 it.second.normal() == Vec2(0, -1)))
				{
					enemy.ToggleIsLookAtDown();
					enemy.turnWatch().restart();
				}
				if (enemy.GetTurnWatch().ms() > 5000)
				{
					enemy.ToggleIsLookAtDown();
					enemy.turnWatch().restart();
				}
				
			}
			for (auto&& enemy : cannonEnemys)
			{
				//X座標の比較で反転フラグを立てる
				if (player.GetBody().getPos().x > enemy.GetBody().getPos().x)
				{
					enemy.SetIsLookAtRight(true);
				}
				else
				{
					enemy.SetIsLookAtRight(false);
				}
			}
			for (auto&& enemy : bulletEnemys)
			{
				//プレイヤーと接触したとき飛翔体が消滅する
				if (it.first.a == player.GetBody().id()
					and it.first.b == enemy.GetBody().id())
				{
					enemy.body().release();

					player.SetIsInvincible(true);
					player.invincibleTimer().start();
					player.DecreaseHealth();

				}
			}
		}
	}

	//無敵時間が2秒超えたらリセット
	if (player.invincibleTimer().ms() > 2000)
	{
		player.SetIsInvincible(false);

		player.invincibleTimer().reset();
		player.body().setVelocity(Vec2(0, 0));
	}

	if (not player.GetIsInvincible())
	{
		ControlPlayer();
	}

	ControlEnemys();

	camera.update();
	Vec2 Center = player.GetBody().getPos();
	//スクロールの制限をいい感じにする　後ろの/2だけ調整するかも
	Center.y = Clamp(Center.y, Scene::CenterF().y, static_cast<double>(CHIP_SIZE * 32) - Scene::CenterF().y / 2);
	camera.setCenter(Center);
	animeUpdate();
}

void Game::draw() const
{
	//TextureAsset(U"sampleBack").scaled(1.5,1.5).draw();
	const auto transf = camera.createTransformer();
	player.GetBody().draw();
	/*TextureAsset(U"dummyPlayer")
		.scaled(0.5, 0.5)
		.draw(player.GetBody().getPos());*/
	uint64 t = player.anime.animeTimer.ms64();
	switch (int32 n; player.anime.animeState)
	{
	{
	case 0:
		//n=(t/1コマあたりの時間％コマの枚数)
		n = (t / 80 % 3);
		//ループさせたくないときは
		//n=(t/1コマあたりの時間/コマの枚数)
		//if(t > 一コマあたりの時間*コマの枚数)
		//n = コマの枚数 - 1
		if (t > 80 * 3)
			n = 2;
		//描画と当たり判定のズレは
		//+Vec2{-CHIP_SIZE*描画スケールｘ,-CHIP_SIZE / 2 * 描画スケールｙ}
		TextureAsset(U"player")
			((player.anime.shrinkSpring[n]) * 256,//x
				player.anime.animeState * 256,//y
				256)//Rect(256)
			.scaled(Vec2{ 0.6,0.6 })
			.draw(player.GetBody().getPos() + Vec2{-CHIP_SIZE*0.6,-CHIP_SIZE /2*0.6});
		break;
	case 1:
		//n=(t/1コマあたりの時間％コマの枚数)
		n = (t / 500 % 3);
		//n=(t/1コマあたりの時間/コマの枚数)
		//ループさせたくないときは
		//if(t > 一コマあたりの時間*コマの枚数)
		//n = コマの枚数 - 1
		if (t > 500 * 3)
			n = 2;
		TextureAsset(U"player")
			((player.anime.extendSpring[n]) * 256,//x
				player.anime.animeState * 256,//y
				256)//Rect(256)
			.scaled(Vec2{ 0.6,0.6 })
			.draw(player.GetBody().getPos() + Vec2{ -CHIP_SIZE * 0.6,-CHIP_SIZE / 2 * 0.6 });
		break;
	case 2:
		//n=(t/1コマあたりの時間％コマの枚数)
		n = (t / 1000 % 3);
		//ループさせたくないときは
		//n=(t/1コマあたりの時間/コマの枚数)
		//if(t > 一コマあたりの時間*コマの枚数)
		//n = コマの枚数 - 1
		if (t > 1000 * 3)
			n = 2;
		TextureAsset(U"player")
			((player.anime.hardShrinkSpring[n]) * 256,//x
				player.anime.animeState * 256,//y
				256)//Rect(256)
			.scaled(Vec2{ 0.6,0.6 })
			.draw(player.GetBody().getPos() + Vec2{ -CHIP_SIZE * 0.6,-CHIP_SIZE / 2 * 0.6 });
		break;
	case 3:
		if ((KeyA.pressed() or KeyLeft.pressed()))
		{
			TextureAsset(U"player")
				((player.anime.attachWall[0]) * 256,//x
					player.anime.animeState * 256,//y
					256)//Rect(256)
				.scaled(Vec2{ 0.6,0.6 })
				.draw(player.GetBody().getPos() + Vec2{ -CHIP_SIZE * 0.6,-CHIP_SIZE / 2 * 0.6 });
		}
		if((KeyD.pressed() or KeyRight.pressed()))
		{
			TextureAsset(U"player")
				((player.anime.attachWall[0]) * 256,//x
					player.anime.animeState * 256,//y
					256)//Rect(256)
				.scaled(Vec2{ 0.6,0.6 })
				.mirrored()
				.draw(player.GetBody().getPos() + Vec2{ -CHIP_SIZE * 0.6,-CHIP_SIZE / 2 * 0.6 });
		}
		break;
	case 4:
		//n=(t/1コマあたりの時間％コマの枚数)
		n = (t / 2000 % 2);
		//ループさせたくないときは
		//n=(t/1コマあたりの時間/コマの枚数)
		//if(t > 一コマあたりの時間*コマの枚数)
		//n = コマの枚数 - 1
		if (t > 2000 * 2)
			n = 1;
		//ダメージ2枚目は分岐が面倒だったので使わない
		TextureAsset(U"player")
			((player.anime.damaged[0]) * 256,//x
				player.anime.animeState * 256,//y
				256)//Rect(256)
			.scaled(Vec2{ 0.6,0.6 })
			.draw(player.GetBody().getPos() + Vec2{ -CHIP_SIZE * 0.6,-CHIP_SIZE / 2 * 0.6 });
		if (t / 200 % 2)//200ms周期
		{

			ScopedColorAdd2D color{ 1.0,1.0,0.1,0 };
			TextureAsset(U"player")
				((player.anime.damaged[0]) * 256,//x
					player.anime.animeState * 256,//y
					256)//Rect(256)
				.scaled(Vec2{ 0.6,0.6 })
				.draw(player.GetBody().getPos() + Vec2{ -CHIP_SIZE * 0.6,-CHIP_SIZE / 2 * 0.6 });
		}
		
		break;
	}
	default:
	break;
	}
	/*TextureAsset(U"player")
		((animePatterns[n]) * 256, (y * 0), 256)
		.scaled(Vec2{0.6,0.6})
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
	Print << player.GetHealth();
}

void Game::ControlPlayer()
{
	

	if (player.landingDelay().ms() > 500)
	{
		if (isInputLeftDirection()
			and player.GetPreviousVelocity().x > 0)
		{
			player.SetPreviousvelocity(Vec2(0, player.GetPreviousVelocity().y));
		}
		if (isInputRightDirection()
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
	
	if (KeySpace.up())
	{
		player.SetIsJumpRestriction(false);
	}
	if (isInputDownDirection())
	{
		player.landingDelay().reset();
		player.body().setDamping(0.99);
	}
	if (isInputDownDirection()
		and (isInputLeftDirection() or isInputRightDirection()))
	{
		player.body().setDamping(0.8);
	}
	if (isReleaseDownDirection())
	{
		player.body().setDamping(0.1);
	}
	if (isReleaseDownDirection()
		and player.GetIsOnGround())
	{
		player.landingDelay().reset();
		player.body().setGravityScale(1);
		player.body().applyLinearImpulse(Vec2(0, player.GetParam().jumpPower * 5));
		//KeyS.upなのでここだけSetFalse
		player.SetIsJumpRestriction(false);
		player.SetIsOnGround(false);
		player.body().setDamping(0.1);
		player.SetShouldRecordVelocity(true);
	}
	if (player.GetIsOnGround()
		and not(player.GetIsJumpRestriction()))
	{
		if ((KeySpace.pressed() or KeySpace.down())
			and isInputLeftDirection())
		{
			player.landingDelay().reset();
			player.body().setVelocity(player.GetPreviousVelocity());
			player.body().setGravityScale(1);
			player.body().applyLinearImpulse(Vec2(-player.GetParam().floorKickPower, player.GetParam().jumpPower / 4));
			player.SetIsJumpRestriction(true);
			player.SetIsOnGround(false);
			player.SetShouldRecordVelocity(true);
		}
		if ((KeySpace.pressed() or KeySpace.down())
			and isInputRightDirection())
		{
			player.landingDelay().reset();
			player.body().setVelocity(player.GetPreviousVelocity());
			player.body().setGravityScale(1);
			player.body().applyLinearImpulse(Vec2(player.GetParam().floorKickPower, player.GetParam().jumpPower / 4));
			player.SetIsJumpRestriction(true);
			player.SetIsOnGround(false);
			player.SetShouldRecordVelocity(true);
		}
		if ((KeySpace.pressed() or KeySpace.down())
			and not(isInputLeftDirection() or isInputRightDirection()))
		{
			player.landingDelay().reset();
			player.body().setVelocity(player.GetPreviousVelocity());
			player.body().setGravityScale(1);
			player.body().applyLinearImpulse(Vec2(0, player.GetParam().jumpPower));
			player.SetIsJumpRestriction(true);
			player.SetIsOnGround(false);
			player.SetShouldRecordVelocity(true);
		}
	}

	if (isInputLeftDirection()
		and not player.GetIsOnGround())
	{
		player.body().applyForce(Vec2(-player.GetParam().accelerationX * Scene::DeltaTime(), 0));
	}
	if (isInputRightDirection()
		and not player.GetIsOnGround())
	{
		player.body().applyForce(Vec2(player.GetParam().accelerationX * Scene::DeltaTime(), 0));
	}

	double vel = Clamp(player.body().getVelocity().x, -player.GetParam().maxVelocity.x, player.GetParam().maxVelocity.x);
	player.body().setVelocity(Vec2(vel, player.body().getVelocity().y));
}

void Game::ResponsePlayerLeftHit()
{
	player.body().setVelocity(Vec2(0, 0));
	player.body().setGravityScale(1);
	player.body().applyLinearImpulse(player.GetParam().knockbackedToRight);
	player.SetShouldRecordVelocity(true);
	player.SetIsOnGround(false);
	player.SetIsInvincible(true);
	player.invincibleTimer().start();
	player.DecreaseHealth();
}

void Game::ResponsePlayerRightHit()
{
	player.body().setVelocity(Vec2(0, 0));
	player.body().setGravityScale(1);
	player.body().applyLinearImpulse(player.GetParam().knockbackedToLeft);
	player.SetShouldRecordVelocity(true);
	player.SetIsOnGround(false);
	player.SetIsInvincible(true);
	player.invincibleTimer().start();
	player.DecreaseHealth();
}

void Game::ResponsePlayerBottomHit()
{
	if (KeySpace.pressed() or KeySpace.down())
	{
		player.landingDelay().reset();
		//playerのPreviousVelocityが上の方でy>0になってしまっているので跳ねない
		//YのVelocityを０に設定し直しておく
		player.body().setVelocity(Vec2(player.GetPreviousVelocity().x, 0));
		player.body().setGravityScale(1);
		player.body().applyLinearImpulse(Vec2(0, -800));
		//Restrictionの挙動がおかしいのでここでFalseにする
		player.SetIsJumpRestriction(false);
		player.SetIsOnGround(false);
		player.SetShouldRecordVelocity(true);
	}
	else
	{
		player.landingDelay().reset();
		//playerのPreviousVelocityが上の方でy>0になってしまっているので跳ねない
		//YのVelocityを０に設定し直しておく
		player.body().setVelocity(Vec2(player.GetPreviousVelocity().x, 0));
		player.body().setGravityScale(1);
		player.body().applyLinearImpulse(Vec2(0, -300));
		//Restrictionの挙動がおかしいのでここでFalseにする
		player.SetIsJumpRestriction(false);
		player.SetIsOnGround(false);
		player.SetShouldRecordVelocity(true);
	}
}


void Game::ControlEnemys()
{
	for (auto&& enemy : walkingEnemys)
	{
		if (enemy.GetIsLookAtRight())
		{
			//enemy.body().applyForce(Vec2(1000000 * Scene::DeltaTime(), 0));
			enemy.body().setVelocity(enemy.GetBody().getVelocity() + Vec2(100, 0));
		}
		else
		{
			//enemy.body().applyForce(Vec2(-(1000000 * Scene::DeltaTime()), 0));
			enemy.body().setVelocity(enemy.GetBody().getVelocity() + Vec2(-100, 0));
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
		double diff = AbsDiff(player.GetBody().getPos().x, enemy.GetBody().getPos().x);

		if (enemy.GetShotInterval().ms() > 5000
			and diff < Scene::Width())
		{
			if (enemy.GetIsLookAtRight())
			{
				BulletEnemy bullet = BulletEnemy(world.createCircleSensor(P2Dynamic, enemy.GetBody().getPos() + Vec2(CHIP_SIZE, CHIP_SIZE / 2), 10)
											, Vec2(100, 0));
				bullet.body().setGravityScale(0);
				bulletEnemys << bullet;
				enemy.shotInterval().restart();
			}
			else
			{
				BulletEnemy bullet = BulletEnemy(world.createCircleSensor(P2Dynamic, enemy.GetBody().getPos() + Vec2(0, CHIP_SIZE / 2), 10)
											, Vec2(-100, 0));
				bullet.body().setGravityScale(0);
				bulletEnemys << bullet;
				enemy.shotInterval().restart();
			}
		}

	}
	for (auto&& bullet : bulletEnemys)
	{
		double diff = AbsDiff(player.GetBody().getPos().x, bullet.GetBody().getPos().x);

		//固定値で動かす
		bullet.body().moveBy(bullet.GetVelocity() * Scene::DeltaTime());
		if (bullet.GetBody().getPos().x < 0
			or diff > Scene::Width())
		{
			bullet.body().release();
		}
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
			//敵とプレイヤーとの中心座標のベクトルを正規化
			bulletEnemys << BulletEnemy(world.createCircleSensor(P2Kinematic, enemy.GetBody().getPos(), 10),
										Normalize((player.GetBody().getPos() + Vec2(CHIP_SIZE / 2, CHIP_SIZE)) - enemy.GetBody().getPos()) * 5);

			enemy.shotInterval().restart();
		}

	}
}
