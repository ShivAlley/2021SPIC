int GetTile(const CSV& gMap, const int rowsize, const int x, const int y)
{
	return Parse<int>(gMap[y][x]);
}
