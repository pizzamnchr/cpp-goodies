#pragma once

#include <iostream>
#include <vector>
#include <string>
using namespace std;

// What is next to the tile
enum class NextToTile { emptyTile, numberTile, mineTile, unknownTile };

class GameTiles
{
private:
	NextToTile inTile = NextToTile::unknownTile;

	unsigned int tileNumber = 0;

	unsigned int xCoordinate = 0;

	unsigned int yCoordinate = 0;

	bool flagTile = false;

	bool showTile = false;

public:
	// Constructor for the game's tiles
	GameTiles();

	// Constructor for the game's tiles with x and y coordinates
	GameTiles(unsigned int xCoord, unsigned int yCoord);

	// Getter for what is inside the tile
	NextToTile getInTile();

	// Getter for the x coordinate of tile
	unsigned int getCoordinateX();

	// Getter for the y coordinate of tile
	unsigned int getCoordinateY();

	// Getter for the tile number
	unsigned int getTileNumber();

	// Setter for what is inside the tile
	void setInTile(NextToTile tileContents);

	// Setter for the tile number
	void setTileNumber(unsigned tileNumbers);

	// Function to show the tile
	void showTheTile();

	// Function to show the tile flag
	void showTheTileFlag();

	// Check if the tile is flagged
	bool isTileFlagged();

	// Check if the tile's contents should be showing
	bool isTileShown();
};
