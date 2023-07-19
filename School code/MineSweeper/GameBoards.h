#pragma once

#include <iostream>
#include <ctime>
#include <vector>
#include <string>
#include <fstream>
#include <random>

#include "GameTiles.h"

using namespace std;

class GameBoards
{
private:

	// Columns for game boards
	unsigned int gameBoardsColumns = 25;

	// Rows for game boards
	unsigned int gameBoardsRows = 16;

	// The number of mines on the game board
	unsigned int gameBoardsMineAmount = 0;

	// The number of flags on the game board
	int gameBoardsFlagsAmount = 0;

	// Bool to check if player clicked on a mine
	bool notMineTile = true;

	// Bool to check if player won the game
	bool didPlayerWin = false;

	// Seeing if the player won the game
	bool playerGameStatus();

	// Vector for the tiles of the game boards
	vector<vector<GameTiles>> _gameBoardTiles;

	// Vector for the rows of the game boards
	vector<string> _gameBoardsRows;

	// Function to read the game board
	void readGameBoard(string inBoard);

	// Function to calculate the numbers for the number tiles
	int calculateNumberTileNumbers(unsigned int boardRow, unsigned int boardColumn);

	// Function to setup the game board
	void setUpGameBoard();

	// Function to empty the game board
	void emptyGameBoard(unsigned int xCoord, unsigned int yCoord);

	// Function to calculate the number of mines on the game board
	void calculateMineAmount();

	// Getter for the game board's mine amount
	unsigned int gameBoardMineAmount();

	// Function to increase the game board's mine amount
	void increaseGameBoardMineAmount();

	// Function to decrease the game board's mine amount
	void decreaseGameBoardMineAmount();

	// Function to place mine tiles on the game board randomly
	void placeGameBoardMineTilesRandom();

	// Function to place mine tiles onto the game board
	void placeGameBoardMineTiles();

	// Function to place number tiles onto the game board
	void placeGameBoardNumberTiles();

	// Function to place empty tiles onto the game board
	void placeGameBoardEmptyTiles();

	// Function to show the mines when the player clicks the toggle debug button
	void showGameBoardMines();              

public:

	// Constructor for the game board
	GameBoards();

	// Getter for a game board tile
	GameTiles& gameBoardsTile(unsigned int boardRow, unsigned int boardColumn);

	// Getter for the game board's flag amount
	int flagCount();

	// Function to take in a game board
	void takeInBoard(string file);

	// Function to check if it is possible to play a game
	bool gameOn();

	// Function to reveal the tile's contents
	void revealTilesContents(unsigned int boardRow, unsigned int boardColumn);

	// Function to create a fresh game board with random mine placements
	void newRandomBoard();

	// Function see player's game status
	bool playerStatus();

	// Function to reset the bool so player can play again
	void playAgain();

	// Function to increase the game board's flag amount
	void increaseGameBoardFlagAmount();

	// Function to decrease the game board's flag amount
	void decreaseGameBoardFlagAmount();
};


