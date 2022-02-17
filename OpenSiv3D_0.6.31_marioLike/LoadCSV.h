#include<Siv3D.hpp>

int GetTile(const CSV& gMap, const int rowsize, const int x, const int y)
{
	return Parse<int>(gMap[y][x]);
}

void LoadMap() {
	constexpr int GridSize = 128;				// マップチップ一個の一辺の長さ
	constexpr int AtlasWidth = 256;				// マップチップ画像全体の横幅
	constexpr int PerLineTiles = 2;				// 一行のマップチップの数

	// テクスチャ読み込み
	Texture gTextureAtlas(U"Image/sample.png");			//マップチップ画像
	//マップデータ読み込み
	const CSV gMap(U"example/csv/sample.csv");

	const int gMapWidth = 32;					//マップデータの横幅
	const int gMapHeight = 32;					//マップデータの縦幅
	const int gTileWidth = GridSize;
	const int gTileHeight = GridSize;

	while (System::Update())
	{
		for (auto y : step(gMapHeight))			
		{
			for (auto x : step(gMapWidth))
	{
				int tile = GetTile(gMap, gMapWidth, x, y);
				gTextureAtlas((GridSize * tile) % AtlasWidth, GridSize * (tile / PerLineTiles), GridSize, GridSize).draw(x * gTileWidth, y * gTileHeight);
			}
	}
	}
}
