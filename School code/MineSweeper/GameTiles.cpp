#include "GameTiles.h"

GameTiles::GameTiles()
{

}

GameTiles::GameTiles(unsigned int xCoord, unsigned int yCoord) : xCoordinate(xCoord), yCoordinate(yCoord)
{

}

NextToTile GameTiles::getInTile()
{ 
	return inTile; 
}

unsigned int GameTiles::getCoordinateX()
{ 
	return xCoordinate; 
}

unsigned int GameTiles::getCoordinateY()
{ 
	return yCoordinate; 
}

unsigned int GameTiles::getTileNumber()
{ 
	return tileNumber; 
}

void GameTiles::setInTile(NextToTile tileContents)
{ 
	inTile = tileContents; 
}

void GameTiles::setTileNumber(unsigned tileNumbers)
{ 
	tileNumber = tileNumbers;
}

void GameTiles::showTheTile()
{ 
	showTile = true; 
}

void GameTiles::showTheTileFlag()
{
	flagTile = !flagTile; 
}

bool GameTiles::isTileFlagged()
{ 
	return flagTile; 
}

bool GameTiles::isTileShown()
{ 
	return showTile; 
}


