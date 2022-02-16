#include "Title.h"
#include "LoadCSV.h"

Title::Title(const InitData& init):IScene(init)
{
}

void Title::update()
{
	LoadCSV();
}

void Title::draw() const
{
}
