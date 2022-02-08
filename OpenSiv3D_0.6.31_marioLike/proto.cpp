# include <Siv3D.hpp> // OpenSiv3D v0.6.3



class Player
{
public:
	Player();
	~Player();
	float getSpeed()const { return speed; }
	Vec2 getPosition()const { return position; }
	P2Body getPlayerBody()const { return playerBody; }
	P2Body setPlayerBody(P2World& world, P2Material& material)
	{
		playerBody = world.createRect(P2Dynamic, Vec2(0, -300), RectF(40, 80), material);
		return playerBody;
	}
private:
	P2Body playerBody;
	Vec2 position = { 0,-600 };
	float speed = 200;

};

Player::Player()
{
}

Player::~Player()
{
}



void Main()
{
	// ウィンドウを 1280x720 にリサイズ
	Window::Resize(1280, 720);
	//MonitorInfo s = System::GetCurrentMonitor();
	//String ss = s.format();
	//Print << ss;
	//Scene::DeltaTime();

	constexpr int8 chip = 40;

	// 2D 物理演算のシミュレーションステップ（秒）
	constexpr double stepSec = (1.0 / 200.0);

	// 2D 物理演算のシミュレーション蓄積時間（秒）
	double accumulatorSec = 0.0;

	// 重力加速度 (cm/s^2)
	constexpr double gravity = 980;

	// 2D 物理演算のワールド
	P2World world{ gravity };

	// [_] 地面 (幅 1200 cm の床）
	//const P2Body ground = world.createLine(P2Static, Vec2{ 0, 0 }, Line{ -600, 0, 600, 0 });

	// 物体
	std::unique_ptr<Player> player(new Player());

	P2Material material;
	material.restitution = 0;
	material.friction = 0.5;
	//P2Body playerBody = world.createRect(P2Dynamic, Vec2(0, -300), RectF(40, 80),material);
	//playerBody.setFixedRotation(true);
	//playerBody.setGravityScale(0);
	player.get()->setPlayerBody(world, material);
	//player.get()->getPlayerBody().addTriangle();
	player.get()->getPlayerBody().setFixedRotation(true);
	player.get()->getPlayerBody().setGravityScale(1.0);
	Array<P2Body> bodies;
	Array<P2Body> chips;
	//Grid<int8> maps(5, 3, -1);
	Grid<int8> map;
	map = {
		{1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	};

	Size size = { map.width(), map.height() };//マップの大きさ｛ｘ、ｙ｝
	for (auto p : step(size)) {
		if (map[p.y][p.x] == 1)
			chips << world.createRect(P2Static, Vec2{ p.x * 40 - 1280 / 2,p.y * 40 - 720 / 2 }, RectF(chip, chip, chip, chip), material);
	}

	

	// 2D カメラ
	Camera2D camera{ Vec2{ 0, 0 } };
	camera.setParameters(Camera2DParameters::NoControl());

	

	while (System::Update())
	{


		const double deltaTime = Scene::DeltaTime();



		for (accumulatorSec += Scene::DeltaTime(); stepSec <= accumulatorSec; accumulatorSec -= stepSec)
		{
			// 2D 物理演算のワールドを更新
			world.update(stepSec);
		}

		if (not (KeyDown | KeyUp | KeyLeft | KeyRight).pressed())
		{
			//player.get()->getPlayerBody().setVelocity(Vec2(0, 0));
			player.get()->getPlayerBody().setVelocity(player.get()->getPlayerBody().getVelocity() * Vec2(0.9, 0.9));
		}
		

		// 地面より下に落ちた物体は削除
		bodies.remove_if([](const P2Body& b) { return (200 < b.getPos().y); });

		// 2D カメラの更新
		camera.update();
		{
			// 2D カメラから Transformer2D を作成
			const auto t = camera.createTransformer();

			// 左クリックしたら
			if (MouseL.down())
			{
				// クリックした場所に半径 10 cm のボールを作成
				//bodies << world.createCircle(P2Dynamic, Cursor::PosF(), 10);
				bodies << world.createTriangle(P2Dynamic, Cursor::PosF(), Triangle(10, 100, 100));
			}

			if (KeyLeft.pressed())
			{
				player.get()->getPlayerBody().setVelocity(Vec2(-player.get()->getSpeed(), 0));
				//playerBody.moveBy(Vec2(-player.get()->getSpeed() * deltaTime, 0));
				//playerBody.setVelocity(Vec2(-player.get()->getSpeed(), 0));
			}

			if (KeyRight.pressed())
			{
				player.get()->getPlayerBody().setVelocity(Vec2(player.get()->getSpeed(), 0));
				//playerBody.moveBy(Vec2(player.get()->getSpeed() * deltaTime, 0));
				//playerBody.setVelocity(Vec2(player.get()->getSpeed(), 0));
			}

			if (KeyUp.pressed())
			{
				player.get()->getPlayerBody().setVelocity(Vec2(0, -player.get()->getSpeed()));
			}

			if (KeyDown.pressed())
			{
				//playerBody.applyForce(Vec2(0, player.get()->getSpeed()));
				player.get()->getPlayerBody().setVelocity(Vec2(0, player.get()->getSpeed()));
			}

			

			//player.get()->getPlayerBody().

			/*if (camera.getCenter().x - 1280/ 2 < player.get()->getPlayerBody().getPos().x - 1280 + 1280 / 2)
				camera.setCenter(Vec2(player.get()->getPlayerBody().getPos().x , camera.getCenter().y));
			if (camera.getCenter().y - 720 / 2 < player.get()->getPlayerBody().getPos().y - 720 + 720 / 2)
				camera.setCenter(Vec2(camera.getCenter().x, player.get()->getPlayerBody().getPos().y));*/
			camera.setTargetCenter(player.get()->getPlayerBody().getPos());

			ClearPrint();
			Print << camera.getCenter();


			//= player.get()->getPosition().x - 1280 + 1280 / 2;
			//if (scroll.x > player.pos.x - player.HalfSize.x - SCROLL_MERGIN_X)
			//	scroll.x = player.pos.x - player.HalfSize.x - SCROLL_MERGIN_X;
			//if (scroll.y < player.pos.y + player.HalfSize.y - SCREEN_H + SCROLL_MERGIN_Y / 1.5f)
			//	scroll.y = player.pos.y + player.HalfSize.y - SCREEN_H + SCROLL_MERGIN_Y / 1.5f;
			//if (scroll.y > player.pos.y - player.HalfSize.y - SCROLL_MERGIN_Y)
			//	scroll.y = player.pos.y - player.HalfSize.y - SCROLL_MERGIN_Y;

			// すべてのボディを描画
			for (const auto& body : bodies)
			{
				body.draw(HSV{ body.id() * 10.0 });
			}

			for (const auto& chip : chips)
			{
				chip.draw(HSV{ chip.id() * 10.0 });
			}

			player.get()->getPlayerBody().draw();
			player.get()->getPlayerBody().drawFrame(1.0, Palette::Red);
			player.get()->getPlayerBody().drawWireframe(1.0, Palette::Cyan);

			// 地面を描画
			//ground.draw(Palette::Skyblue);
		}


		// 2D カメラの操作を描画
		camera.draw(Palette::Orange);
	}
}
