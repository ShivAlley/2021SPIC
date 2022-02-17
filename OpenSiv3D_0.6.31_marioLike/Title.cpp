#include "Title.h"
#include "LoadCSV.h"

Title::Title(const InitData& init):IScene(init)
{
}

void Title::update()
{
	LoadMap();
}

void Title::draw() const
{	
}
