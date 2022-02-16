#pragma once
#include <Siv3D.hpp>
//experimental implementation
#include "Delegater.h"
enum class GameState
{
	Title,
	Menu,
	Game,
};

struct ShareGameData
{
	int dummy;
};

using App = SceneManager<GameState, ShareGameData>;



static constexpr int32 CHIP_SIZE = 64;
// 2D 物理演算のシミュレーションステップ（秒）
static constexpr double stepSec = (1.0 / 200.0);

// 2D 物理演算のシミュレーション蓄積時間（秒）
static double accumulatorSec = 0.0;

static constexpr double gravity = 980;

