# include <Siv3D.hpp> // OpenSiv3D v0.6.3



# include "Title.h"
# include "Game.h"
# include "Menu.h"

void Main()
{
	Window::SetToggleFullscreenEnabled(true);
	Window::SetFullscreen(true,0);
	Scene::SetResizeMode(ResizeMode::Keep);
	//TextureAsset::Register(U"sampleBack", U"Image/sampleBack.png");
	TextureAsset::Register(U"dummy256", U"Image/256x256.png");
	TextureAsset::Register(U"dummyPlayer", U"Image/player.png",TextureDesc::Mipped);
	App manager;
	manager.add<Title>(GameState::Title);
	manager.add<Game>(GameState::Game);
	manager.add<Menu>(GameState::Menu);

	// ゲームシーンから開始
	manager.init(GameState::Game);

	while (System::Update())
	{
		if (not manager.update())
		{
			break;
		}
	}
}
