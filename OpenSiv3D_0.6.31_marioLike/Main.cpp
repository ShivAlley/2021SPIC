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
	TextureAsset::Register(U"player", U"Image/costomPlayer.png",TextureDesc::Mipped);
	TextureAsset::Register(U"terrain", U"Image/map.png", TextureDesc::Mipped);
	TextureAsset::Register(U"coin", U"Image/jewel.png", TextureDesc::Mipped);
	TextureAsset::Register(U"bullet", U"Image/bullet.png", TextureDesc::Mipped);
	TextureAsset::Register(U"walkEnemy", U"Image/ground_enemy.png", TextureDesc::Mipped);
	TextureAsset::Register(U"flyEnemy", U"Image/sky_enemy.png", TextureDesc::Mipped);
	TextureAsset::Register(U"cannonEnemy", U"Image/shot_enemy.png", TextureDesc::Mipped);
	TextureAsset::Register(U"stomp", U"Image/stomp_effect.png", TextureDesc::Mipped);
	TextureAsset::Register(U"back", U"Image/back_ground.png", TextureDesc::Mipped);
	AudioAsset::Register(U"bound", U"Music/bound.mp3");
	AudioAsset::Register(U"coin", U"Music/coin.mp3");
	AudioAsset::Register(U"decide", U"Music/decide.mp3");
	AudioAsset::Register(U"stomp", U"Music/stomp.mp3");
	AudioAsset::Register(U"gameover", U"Music/game_over.mp3");
	//Loop
	AudioAsset::Register(U"title", U"Music/title_bgm.mp3", Loop::Yes);
	AudioAsset::Register(U"gameClear", U"Music/game_clear.mp3", Loop::Yes);
	AudioAsset::Register(U"game", U"Music/game_bgm.mp3", Loop::Yes);
	App manager;
	manager.add<Title>(GameState::Title);
	manager.add<Game>(GameState::Game);
	manager.add<Menu>(GameState::Menu);
	manager.add<Gameover>(GameState::Gameover);

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
