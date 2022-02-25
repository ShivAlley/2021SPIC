#pragma once
#include <Siv3D.hpp>
//experimental implementation
#include "Delegater.h"
enum class GameState
{
	Title,
	Menu,
	Game,
	Gameover,
};

struct ShareGameData
{
	int dummy = 0;
	int32 Life{3};
	int32 stageNum = 0;
};

using App = SceneManager<GameState, ShareGameData>;



static constexpr int32 CHIP_SIZE = 64;
static constexpr int32 BODY_SIZE = 64 - 16;
// 2D 物理演算のシミュレーションステップ（秒）
static constexpr double STEP_SEC = (1.0 / 200.0);

// 2D 物理演算のシミュレーション蓄積時間（秒）
static double ACCUMULATOR_SEC = 0.0;

static constexpr double GRAVITY = 980;
/// @brief マップチップ画像全体の横幅
static constexpr int32 ATLAS_WIDTH = 256;
/// @brief 一行のマップチップの数
static constexpr int32 PER_LINE_TILES = 2;

