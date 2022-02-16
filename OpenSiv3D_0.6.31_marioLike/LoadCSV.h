#include<Siv3D.hpp>

struct Item
{
	String label;

	Point pos;
};

//CSVファイル読み込み
void LoadCSV() {

	const CSV csv{ U"example/csv/config.csv" };

	//エラーチェック
	if (not csv)
	{
		throw Error{ U"Failed to load csv" };
	}

	for (size_t row = 0; row < csv.rows(); ++row) {
		Print << csv[row];
	}
	Print << U"--------";
}
