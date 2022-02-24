# include <Siv3D.hpp> // OpenSiv3D v0.6.3



# include "Title.h"
# include "Game.h"
# include "Menu.h"
# include "Gameover.h"

void Main()
{
	Window::SetToggleFullscreenEnabled(true);
	Window::SetFullscreen(true,0);
	Scene::SetResizeMode(ResizeMode::Keep);
	//TextureAsset::Register(U"sampleBack", U"Image/sampleBack.png");
	//TextureAsset::Register(U"dummy256", U"Image/256x256.png");
	//TextureAsset::Register(U"dummyPlayer", U"Image/player.png",TextureDesc::Mipped);
	//TextureAsset::Register(U"dummychip", U"Image/sample.png");
	FontAsset::Register(U"Title", 60, FileSystem::GetFolderPath(SpecialFolder::SystemFonts) + U"UDDigiKyokashoN-B.ttc");
	TextureAsset::Register(U"player", U"Image/costomPlayer.png",TextureDesc::Mipped);
	TextureAsset::Register(U"terrain", U"Image/map.png", TextureDesc::Mipped);
	TextureAsset::Register(U"coin", U"Image/jewel.png", TextureDesc::Mipped);
	TextureAsset::Register(U"bullet", U"Image/bullet.png", TextureDesc::Mipped);
	App manager;
	manager.add<Title>(GameState::Title);
	manager.add<Game>(GameState::Game);
	manager.add<Menu>(GameState::Menu);
	manager.add<Gameover>(GameState::Gameover);

	// ゲームシーンから開始
	manager.init(GameState::Title);

	while (System::Update())
	{
		if (not manager.update())
		{
			break;
		}
	}
}
