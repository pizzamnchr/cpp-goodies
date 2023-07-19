#include "GameBoards.h"

// Private

bool GameBoards::playerGameStatus()
{
	// The number of tiles that are currently unrevealed
	unsigned int unRevealedTiles = 0;

	// For loop to go through the game board's rows
	for (unsigned int boardRows = 0; boardRows < gameBoardsRows; boardRows++) 
	{
		// For loop to go through the game board's columns
		for (unsigned int boardColumns = 0; boardColumns < gameBoardsColumns; boardColumns++) 
		{
			GameTiles& boardTile = _gameBoardTiles[boardRows][boardColumns];

			// If the player clicks on a game board tile and reveals it, increase the count
			if (boardTile.isTileShown())
				unRevealedTiles++;
		}
	}

	// If the player reveals all of the game board tiles except the mines, they win the game
	if (unRevealedTiles == 400 - gameBoardsMineAmount)
		return true;
	else							   
		return false;
}

void GameBoards::readGameBoard(string inBoard)
{
	// Temporary game board boardRow
	string temporaryGameBoardRow;

	// Taking in the game board
	ifstream ifs { inBoard };

	// Taking in the game board rows
	while 
		(getline(ifs, temporaryGameBoardRow)) _gameBoardsRows.push_back(temporaryGameBoardRow);
}

int GameBoards::calculateNumberTileNumbers(unsigned int boardRow, unsigned int boardColumn)
{
	NextToTile topTile = NextToTile::unknownTile;

	NextToTile leftTile = NextToTile::unknownTile;

	NextToTile rightTile = NextToTile::unknownTile;

	NextToTile bottomTile = NextToTile::unknownTile;

	NextToTile topLeftTile = NextToTile::unknownTile;

	NextToTile topRightTile = NextToTile::unknownTile;

	NextToTile bottomLeftTile = NextToTile::unknownTile;

	NextToTile bottomRightTile = NextToTile::unknownTile;

	unsigned int lastGameBoardColumn = _gameBoardTiles[boardRow].size() - 1;

	unsigned int lastGameBoardRow = _gameBoardTiles.size() - 1;

	unsigned int minesAmount = 0;

	// Getting the tile's contents
	if (boardRow != 0)                          
		topTile = _gameBoardTiles[boardRow - 1][boardColumn].getInTile();

	if (boardColumn != 0)
		leftTile = _gameBoardTiles[boardRow][boardColumn - 1].getInTile();

	if (boardColumn != lastGameBoardColumn)
		rightTile = _gameBoardTiles[boardRow][boardColumn + 1].getInTile();

	if (boardRow != lastGameBoardRow)                    
		bottomTile = _gameBoardTiles[boardRow + 1][boardColumn].getInTile();

	if (boardRow != 0 && boardColumn != 0)
		topLeftTile = _gameBoardTiles[boardRow - 1][boardColumn - 1].getInTile();

	if (boardRow != 0 && boardColumn != lastGameBoardColumn)        
		topRightTile = _gameBoardTiles[boardRow - 1][boardColumn + 1].getInTile();

	if (boardRow != lastGameBoardRow && boardColumn != 0)
		bottomLeftTile = _gameBoardTiles[boardRow + 1][boardColumn - 1].getInTile();

	if (boardRow != lastGameBoardRow && boardColumn != lastGameBoardColumn)  
		bottomRightTile = _gameBoardTiles[boardRow + 1][boardColumn + 1].getInTile();

	// Looking for adjacent mine tiles
	if (topTile == NextToTile::mineTile) 
		minesAmount++;

	if (leftTile == NextToTile::mineTile)
		minesAmount++;

	if (rightTile == NextToTile::mineTile)
		minesAmount++;

	if (bottomTile == NextToTile::mineTile) 
		minesAmount++;

	if (topLeftTile == NextToTile::mineTile)
		minesAmount++;

	if (topRightTile == NextToTile::mineTile) 
		minesAmount++;

	if (bottomLeftTile == NextToTile::mineTile) 
		minesAmount++;

	if (bottomRightTile == NextToTile::mineTile) 
		minesAmount++;

	return minesAmount;
}

void GameBoards::setUpGameBoard()
{
	for (unsigned int boardRow = 0; boardRow < gameBoardsRows; boardRow++) 
	{
		// Vector for the game board's rows
		vector<GameTiles> boardRows;

		for (unsigned int boardColumns = 0; boardColumns < gameBoardsColumns; boardColumns++) 
		{
			GameTiles gameBoardTile(boardColumns, boardRow);

			// Add tiles to the game board's rows vector
			boardRows.push_back(gameBoardTile);
		}

		_gameBoardTiles.push_back(boardRows);
	}
}

void GameBoards::emptyGameBoard(unsigned int xCoord, unsigned int yCoord)
{
	GameTiles topTile;

	GameTiles leftTile;

	GameTiles rightTile;

	GameTiles bottomTile;

	GameTiles topLeftTile;

	GameTiles topRightTile;

	GameTiles bottomLeftTile;

	GameTiles bottomRightTile;

	unsigned int gameBoardColumn = xCoord;

	unsigned int gameBoardRow = yCoord;

	unsigned int gameBoardLastColumn = _gameBoardTiles[gameBoardRow].size() - 1;

	unsigned int gameBoardLastRow = _gameBoardTiles.size() - 1;

	if (gameBoardRow != 0)                          
		topTile = _gameBoardTiles[gameBoardRow - 1][gameBoardColumn];

	if (gameBoardColumn != 0)
		leftTile = _gameBoardTiles[gameBoardRow][gameBoardColumn - 1];

	if (gameBoardColumn != gameBoardLastColumn)
		rightTile = _gameBoardTiles[gameBoardRow][gameBoardColumn + 1];

	if (gameBoardRow != gameBoardLastRow)                    
		bottomTile = _gameBoardTiles[gameBoardRow + 1][gameBoardColumn];

	if (gameBoardRow != 0 && gameBoardColumn != 0)
		topLeftTile = _gameBoardTiles[gameBoardRow - 1][gameBoardColumn - 1];

	if (gameBoardRow != 0 && gameBoardColumn != gameBoardLastColumn)        
		topRightTile = _gameBoardTiles[gameBoardRow - 1][gameBoardColumn + 1];

	if (gameBoardRow != gameBoardLastRow && gameBoardColumn != 0)
		bottomLeftTile = _gameBoardTiles[gameBoardRow + 1][gameBoardColumn - 1];

	if (gameBoardRow != gameBoardLastRow && gameBoardColumn != gameBoardLastColumn)  
		bottomRightTile = _gameBoardTiles[gameBoardRow + 1][gameBoardColumn + 1];

	vector<GameTiles> gameTile = { topTile, bottomTile, leftTile, rightTile, topRightTile, topLeftTile, bottomRightTile, bottomLeftTile };

	for (GameTiles& gameTiles : gameTile) 
	{
		// If shown, carry on
		if (gameTiles.isTileShown()) 
			continue;
		switch (gameTiles.getInTile()) 
		{
		case NextToTile::unknownTile: 
			continue;

		case NextToTile::emptyTile: 

		// If number gameTiles, reveal its contents
		case NextToTile::numberTile:
			revealTilesContents(gameTiles.getCoordinateX(), gameTiles.getCoordinateY());
		}
	}
}

void GameBoards::calculateMineAmount()
{
	// For loop going through the game board's rows
	for (unsigned int boardRows = 0; boardRows < gameBoardsRows; boardRows++) 
	{
		// For loop going through the game board's columns
		for (unsigned int boardColumns = 0; boardColumns < gameBoardsColumns; boardColumns++) 
		{
			GameTiles& gameTile = _gameBoardTiles[boardRows][boardColumns];

			// If tile's contents are a mine, increase the mine amount
			if (gameTile.getInTile() == NextToTile::mineTile) 
				gameBoardsMineAmount++;
		}
	}
}

unsigned int GameBoards::gameBoardMineAmount() 
{ 
	return gameBoardsMineAmount; 
}

void GameBoards::increaseGameBoardMineAmount() 
{ 
	gameBoardsMineAmount++; 
}

void GameBoards::decreaseGameBoardMineAmount() 
{ 
	gameBoardsMineAmount--; 
}

void GameBoards::placeGameBoardMineTilesRandom()
{
	// Getting a random number
	unsigned int randomNumber = (unsigned int)time(0);

	// Calculating random number
	default_random_engine calculatedRandomNumber(randomNumber);

	// Generate a 0 or a 1
	uniform_int_distribution<unsigned int> oneOrZero(0, 1);

	// Mine distribution for game board column
	uniform_int_distribution<unsigned int> gameBoardColumnDistribution(0, 24);

	// Mine distribution for game board row
	uniform_int_distribution<unsigned int> gameBoardRowDistribution(0, 15);

	unsigned int mines = 0;

	while (mines < 50) 
	{
		// Getting a random 0 or 1
		unsigned int randomBinaryNumber = oneOrZero(calculatedRandomNumber);

		// Getting a random column from game board
		unsigned int boardColumns = gameBoardColumnDistribution(calculatedRandomNumber);

		// Getting a random row from game board
		unsigned int boardRows = gameBoardRowDistribution(calculatedRandomNumber);

		GameTiles& gameTile = this->gameBoardsTile(boardRows, boardColumns);

		NextToTile inTile = this->gameBoardsTile(boardRows, boardColumns).getInTile();

		// If a 1 is generated + the tile's contents is not a mine, place a mine in the tile, else, onto the next tile
		if (randomBinaryNumber == 1 && inTile != NextToTile::mineTile)
		{
			gameTile.setInTile(NextToTile::mineTile);

			mines++;
		}
		else 
			continue;
	}
}

void GameBoards::placeGameBoardMineTiles()
{
	// For loop going through the game boards rows
	for (unsigned int boardRows = 0; boardRows < gameBoardsRows; boardRows++) 
	{
		// For loop going through the game boards columns
		for (unsigned int boardColumns = 0; boardColumns < gameBoardsColumns; boardColumns++) 
		{
			GameTiles& gameTile = _gameBoardTiles[boardRows][boardColumns];

			// Calculating tile location on the game board
			char gameBoardLocation = _gameBoardsRows[boardRows][boardColumns];

			// Place mines on the tiles where the 1s are from the test board files
			if (gameBoardLocation == '1')
				gameTile.setInTile(NextToTile::mineTile);
		}
	}
}

void GameBoards::placeGameBoardNumberTiles()
{
	// For loop going through the game boards rows
	for (unsigned int boardRows = 0; boardRows < gameBoardsRows; boardRows++) 
	{
		// For loop going through the game boards columns
		for (unsigned int boardColumns = 0; boardColumns < gameBoardsColumns; boardColumns++) 
		{
			GameTiles& gameTile = _gameBoardTiles[boardRows][boardColumns];

			// If the tile's contents are not a mine, place a number in the tile
			if (gameTile.getInTile() != NextToTile::mineTile)
			{
				// Calculate the tile number
				int gameBoardTileNumber = calculateNumberTileNumbers(boardRows, boardColumns);

				// Place the number in the tile
				gameTile.setTileNumber(gameBoardTileNumber);

				gameTile.setInTile(NextToTile::numberTile);
			}
		}
	}
}

void GameBoards::placeGameBoardEmptyTiles()
{
	// For loop going through the game boards rows
	for (unsigned int boardRows = 0; boardRows < gameBoardsRows; boardRows++) 
	{
		// For loop going through the game boards columns
		for (unsigned int boardColumns = 0; boardColumns < gameBoardsColumns; boardColumns++) 
		{
			GameTiles& gameTile = _gameBoardTiles[boardRows][boardColumns];

			// If the tile's contents is not a number or a mine, make it empty
			if (gameTile.getInTile() != NextToTile::mineTile && gameTile.getTileNumber() == 0) 
			{
				gameTile.setInTile(NextToTile::emptyTile);
			}
		}
	}
}

void GameBoards::showGameBoardMines()
{
	// Total mines on the game board
	int gameBoardMineAmount = 0;

	// For loop going through the game boards rows
	for (unsigned int boardRows = 0; boardRows < gameBoardsRows; boardRows++) 
	{
		// For loop going through the game boards columns
		for (unsigned int boardColumns = 0; boardColumns < gameBoardsColumns; boardColumns++) 
		{
			GameTiles& gameTile = _gameBoardTiles[boardRows][boardColumns];

			// If the contents of the tile is a mine, show the tile
			if (gameTile.getInTile() == NextToTile::mineTile) 
			{
				gameTile.showTheTile();

				// If the tile is flagged, show the flag
				if (gameTile.isTileFlagged())
					gameTile.showTheTileFlag();
			}
		}
	}
}

// Public

GameBoards::GameBoards()
{
	// Taking in the game board file
	readGameBoard("boards/testboard1.brd");

	// Setting up the game board in accordance to it's contents
	setUpGameBoard();

	// Placing down mine tiles onto the game board in random order
	placeGameBoardMineTilesRandom();

	// Placing down number tiles onto the game board
	placeGameBoardNumberTiles();

	// Placing down empty tiles onto the game board
	placeGameBoardEmptyTiles();

	// Calculating mine amount on the game board
	calculateMineAmount();
}

GameTiles& GameBoards::gameBoardsTile(unsigned int boardRow, unsigned int boardColumn)
{
	// Getting game board's tiles
	return _gameBoardTiles[boardRow][boardColumn];
}

int GameBoards::flagCount()
{
	// Getting game board's flag amount
	return ((int)gameBoardsMineAmount - gameBoardsFlagsAmount);
}

void GameBoards::takeInBoard(string file)
{
	// When taking in a new game board, wipe out the old one
	_gameBoardTiles.clear();
	_gameBoardsRows.clear();

	// Getting ready for a new game
	didPlayerWin = false;
	gameBoardsFlagsAmount = 0;
	gameBoardsMineAmount = 0;

	// Reading the given game board files
	readGameBoard(file);

	// Setting up the game board tiles
	setUpGameBoard();

	// Placing down the mine tiles onto the game board
	placeGameBoardMineTiles();

	// Placing down the number tiles onto the game board
	placeGameBoardNumberTiles();

	// Placing down the empty tiles onto the game board
	placeGameBoardEmptyTiles();

	// Calculating the game board's mine amount
	calculateMineAmount();
}

bool GameBoards::gameOn() 
{ 
	// See if game should continue
	return notMineTile; 
}

void GameBoards::revealTilesContents(unsigned int boardRow, unsigned int boardColumn)
{
	GameTiles& gameTile = _gameBoardTiles[boardColumn][boardRow];

	// If the game board tile is not flagged, reveal it
	if (!gameTile.isTileFlagged())
		gameTile.showTheTile();

	// If the game board is empty and not flagged, empty it
	if (gameTile.getInTile() == NextToTile::emptyTile && !gameTile.isTileFlagged())
		emptyGameBoard(boardRow, boardColumn);

	// If the game board tile is a mile tile, show all the mines on the game board 
	if (gameTile.getInTile() == NextToTile::mineTile)
	{
		// Updating the bool
		notMineTile = false;

		// Showing all the mines on the game board
		showGameBoardMines();
	}

	// If the player won, show all the mines on the game board + update the bool
	if (playerGameStatus()) 
	{
		// Showing all the mines on the game board
		showGameBoardMines();

		// Updating the bool
		didPlayerWin = true;
	}
}

void GameBoards::newRandomBoard()
{
	// Emptying the current game board so the new random game board can be put in
	_gameBoardTiles.clear();
	_gameBoardsRows.clear();

	// Getting ready for a new game
	didPlayerWin = false;
	gameBoardsFlagsAmount = 0;
	gameBoardsMineAmount = 0;

	// Taking in and reusing the layout of the test game board
	readGameBoard("boards/testboard1.brd");

	// Setting up the game board tiles
	setUpGameBoard();

	// Placing down the mine tiles onto the game board randomly
	placeGameBoardMineTilesRandom();

	// Placing down the number tiles onto the game board
	placeGameBoardNumberTiles();

	// Placing down the empty tiles onto the game board
	placeGameBoardEmptyTiles();

	// Calculating the game board's mine amount
	calculateMineAmount();
}

bool GameBoards::playerStatus() 
{ 
	// See if player won
	return didPlayerWin; 
}

void GameBoards::playAgain() 
{ 
	// See if player can play again
	notMineTile = true; 
}

void GameBoards::increaseGameBoardFlagAmount() 
{ 
	gameBoardsFlagsAmount++; 
}

void GameBoards::decreaseGameBoardFlagAmount() 
{ 
	gameBoardsFlagsAmount--; 
}




