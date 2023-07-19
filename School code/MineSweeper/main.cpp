#include <iostream>

#include "GameBoards.h"
#include "GameTextures.h"

using namespace std;

int main()
{
	// Creating a new game board
	GameBoards gameBoard;

	// Rendering the game window
	sf::RenderWindow gameWindow(sf::VideoMode(800, 600), "Minesweeper!");

	// Function prototypes for player clicks
	void mouseLeftClick(GameBoards&, sf::RenderWindow&, bool&);

	void mouseRightClick(GameBoards&, sf::RenderWindow&);

	// Function prototype for checking what the player clicked on

	void playerClickedOn(GameTiles&, NextToTile&, GameBoards&, sf::RenderWindow&, float&, float&);

	// Function prototypes for the mine count display, the faces, + the various buttons in the game window

	void displayGameBoardMineCount(GameBoards&, sf::RenderWindow&, float&, unsigned int&);

	void displayGameBoardFaces(GameBoards&, sf::RenderWindow&, float&, unsigned int&);

	void displayGameBoardButtons(sf::RenderWindow&, float&, unsigned int&);

	// Bool for if the player wants to toggle the debug mode + see all the mines on the game board
	bool toggleDebugMode = false;

	// Loading the textures for the game board's tiles
	mineTileTexture.loadFromFile("images/mine.png");
	gameFlagTexture.loadFromFile("images/flag.png");
	revealedTileTexture.loadFromFile("images/tile_revealed.png");
	hiddenTileTexture.loadFromFile("images/tile_hidden.png");

	// Loading the texture for the game board's faces
	winningFaceTexture.loadFromFile("images/face_win.png");
	losingFaceTexture.loadFromFile("images/face_lose.png");
	happyFaceTexture.loadFromFile("images/face_happy.png");
	
	// Loading the texture for the toggle debug button
	toggleDebugButtonTexture.loadFromFile("images/debug.png");

	// Loading the texture for the game digits
	gameDigitTexture.loadFromFile("images/digits.png");

	// Loading the textures for all of the game board's digits
	gameDigitOneTexture.loadFromFile("images/number_1.png");
	gameDigitTwoTexture.loadFromFile("images/number_2.png");
	gameDigitThreeTexture.loadFromFile("images/number_3.png");
	gameDigitFourTexture.loadFromFile("images/number_4.png");
	gameDigitFiveTexture.loadFromFile("images/number_5.png");
	gameDigitSixTexture.loadFromFile("images/number_6.png");
	gameDigitSevenTexture.loadFromFile("images/number_7.png");
	gameDigitEightTexture.loadFromFile("images/number_8.png");

	// Loading the textures for the game board's test buttons
	testOneButtonTexture.loadFromFile("images/test_1.png");
	testTwoButtonTexture.loadFromFile("images/test_2.png");
	testThreeButtonTexture.loadFromFile("images/test_3.png");

	// Binding the textures for the game board's tiles
	mineTileShape.setTexture(&mineTileTexture);
	gameFlagShape.setTexture(&gameFlagTexture);
	revealedTileShape.setTexture(&revealedTileTexture);
	hiddenTileShape.setTexture(&hiddenTileTexture);

	// Binding the texture for the game board's faces
	winningFaceShape.setTexture(&winningFaceTexture);
	losingFaceShape.setTexture(&losingFaceTexture);
	happyFaceShape.setTexture(&happyFaceTexture);

	// Binding the texture for the toggle debug button
	toggleDebugButtonShape.setTexture(&toggleDebugButtonTexture);

	// Binding the texture for the game digits
	gameDigitShape.setTexture(&gameDigitTexture);

	// Binding the textures for all of the game board's digits
	gameDigitOneShape.setTexture(&gameDigitOneTexture);
	gameDigitTwoShape.setTexture(&gameDigitTwoTexture);
	gameDigitThreeShape.setTexture(&gameDigitThreeTexture);
	gameDigitFourShape.setTexture(&gameDigitFourTexture);
	gameDigitFiveShape.setTexture(&gameDigitFiveTexture);
	gameDigitSixShape.setTexture(&gameDigitSixTexture);
	gameDigitSevenShape.setTexture(&gameDigitSevenTexture);
	gameDigitEightShape.setTexture(&gameDigitEightTexture);

	// Binding the textures for the game board's test buttons
	testOneButtonShape.setTexture(&testOneButtonTexture);
	testTwoButtonShape.setTexture(&testTwoButtonTexture);
	testThreeButtonShape.setTexture(&testThreeButtonTexture);

	// Do everything while the window is not closed
	while (gameWindow.isOpen())
	{
		// Variable for a window event
		sf::Event windowEvent;

		// See if any window events occurred
		while (gameWindow.pollEvent(windowEvent))
		{
			// If the player exits and closes the window
			if (windowEvent.type == sf::Event::Closed)
				gameWindow.close();

			// If the player clicks a mouse button
			if (windowEvent.type == sf::Event::MouseButtonPressed)
			{
				// If the player clicks the left button on their mouse
				if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
					mouseLeftClick(gameBoard, gameWindow, toggleDebugMode);

				// If the player clicks the right button on their mouse
				if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
					mouseRightClick(gameBoard, gameWindow);
			}
		}

		// Emptying the game window before displaying the game board
		gameWindow.clear();

		// For loop to setup + display the game board
		for (unsigned int boardRow = 0; boardRow < 16; boardRow++) 
		{
			for (unsigned int boardColumn = 0; boardColumn < 25; boardColumn++) 
			{
				GameTiles& gameTile = gameBoard.gameBoardsTile(boardRow, boardColumn);

				// Variable for the next board column
				float nextBoardColumn = gameTileSize * boardColumn;

				// Variable for the next board row
				float nextBoardRow = gameTileSize * boardRow;

				NextToTile inTile = gameTile.getInTile();

				// Displaying the game board's mine count
				if (boardRow == 15 && boardColumn == 0)
					displayGameBoardMineCount(gameBoard, gameWindow, nextBoardColumn, boardRow);

				// Displaying the game board's faces
				if (boardRow == 15 && boardColumn == 11)
					displayGameBoardFaces(gameBoard, gameWindow, nextBoardColumn, boardRow);

				// Displaying the game board's buttons
				if (boardRow == 15 && boardColumn == 15)
					displayGameBoardButtons(gameWindow, nextBoardColumn, boardRow);

				// If player clicks on a tile, show what is inside it, else draw it hidden
				if (gameTile.isTileShown()) 
				{
					playerClickedOn(gameTile, inTile, gameBoard, gameWindow, nextBoardColumn, nextBoardRow);
				}
				else 
				{
					hiddenTileShape.setPosition(0.0f + nextBoardColumn, 0.0f + nextBoardRow);

					gameWindow.draw(hiddenTileShape);
				}

				// If a tile is flagged by the player, draw a flag on it
				if (gameTile.isTileFlagged()) 
				{
					gameFlagShape.setPosition(0.0f + nextBoardColumn, 0.0f + nextBoardRow);

					gameWindow.draw(gameFlagShape);
				}

				// If the player toggles the debug mode, draw the mines for all the mine tiles
				if (toggleDebugMode && inTile == NextToTile::mineTile && !gameBoard.playerStatus())
				{
					mineTileShape.setPosition(0.0f + nextBoardColumn, 0.0f + nextBoardRow);

					gameWindow.draw(mineTileShape);
				}
			}
		}

		// Displaying the game board
		gameWindow.display();
	}
}

// Function definition for when the player clicks the left button on their mouse
void mouseLeftClick(GameBoards& gameBoard, sf::RenderWindow& gameWindow, bool& toggleDebugMode)
{
	// Getting the position of the player's mouse cursor
	sf::Vector2i cursorPosition = sf::Mouse::getPosition(gameWindow);

	// While the game is on, see where the player's mouse cursor is on the game board
	if (gameBoard.gameOn() && cursorPosition.x >= 0 && cursorPosition.x <= 800 && cursorPosition.y >= 0 && cursorPosition.y <= 510) 
	{
		unsigned int boardColumn = ((unsigned int)cursorPosition.x / 32);

		unsigned int boardRow = ((unsigned int)cursorPosition.y / 32);

		// If the game board tile is not a flagged tile, show it's contents
		if (!gameBoard.gameBoardsTile(boardRow, boardColumn).isTileFlagged())
			gameBoard.revealTilesContents(boardColumn, boardRow);
	}
	// If the player wants to play with a new random game board
	else if (cursorPosition.x >= 352 && cursorPosition.x <= (352 + gameButtonSize) && cursorPosition.y > 510 && cursorPosition.y <= (510 + gameButtonSize)) 
	{
		// Make a new random game board
		gameBoard.newRandomBoard();

		if (!gameBoard.gameOn())
			gameBoard.playAgain();
	}
	// If the player wants to toggle the debug mode for the game board + see all the mines
	else if (cursorPosition.x >= 480 && cursorPosition.x <= (480 + gameButtonSize) && cursorPosition.y > 510 && cursorPosition.y <= (510 + gameButtonSize))
	{
		toggleDebugMode = !toggleDebugMode;
	}

	// If the player wants to play with testboard1
	else if (cursorPosition.x > 544 && cursorPosition.x <= (544 + gameButtonSize) && cursorPosition.y > 510 && cursorPosition.y <= (510 + gameButtonSize))
	{
		// Take in testboard1
		gameBoard.takeInBoard("boards/testboard1.brd");

		if (!gameBoard.gameOn())
			gameBoard.playAgain();
	}

	// If the player wants to play with testboard2
	else if (cursorPosition.x > 608 && cursorPosition.x <= (608 + gameButtonSize) && cursorPosition.y > 510 && cursorPosition.y <= (510 + gameButtonSize))
	{
		// Take in testboard2
		gameBoard.takeInBoard("boards/testboard2.brd");

		if (!gameBoard.gameOn())
			gameBoard.playAgain();
	}

	// If the player wants to play with testboard3
	else if (cursorPosition.x > 672 && cursorPosition.x <= (672 + gameButtonSize) && cursorPosition.y > 510 && cursorPosition.y <= (510 + gameButtonSize))
	{
		// Take in testboard3
		gameBoard.takeInBoard("boards/testboard3.brd");

		if (!gameBoard.gameOn())
			gameBoard.playAgain();
	}
}

// Function definition for when the player clicks the right button on their mouse
void mouseRightClick(GameBoards& gameBoard, sf::RenderWindow& gameWindow)
{
	// Getting the position of the player's mouse cursor
	sf::Vector2i cursorPosition = sf::Mouse::getPosition(gameWindow);

	// While the game is on, see where the player's mouse cursor is on the game board
	if (gameBoard.gameOn() && cursorPosition.x >= 0 && cursorPosition.x <= 800 && cursorPosition.y >= 0 && cursorPosition.y <= 510)
	{
		unsigned int boardColumn = ((unsigned int)cursorPosition.x / 32);

		unsigned int boardRow = ((unsigned int)cursorPosition.y / 32);

		// As long as the game board tile is still hidden, the player can put a flag on it
		if (!gameBoard.gameBoardsTile(boardRow, boardColumn).isTileShown()) 
		{
			gameBoard.gameBoardsTile(boardRow, boardColumn).showTheTileFlag();

			// If the game board tile is flagged, increase the flag amount, else decrease it
			if (gameBoard.gameBoardsTile(boardRow, boardColumn).isTileFlagged())
				gameBoard.increaseGameBoardFlagAmount();
			else
				gameBoard.decreaseGameBoardFlagAmount();
		}
	}
}

// Function definition for when the player clicks on something in the game board
void playerClickedOn(GameTiles& gameTile, NextToTile& inTile, GameBoards& gameBoard, sf::RenderWindow& gameWindow, float& nextBoardColumn, float& nextBoardRow)
{
	// If the player clicks on a currently hidden tile, draw the revealed gameTile
	revealedTileShape.setPosition(0.0f + nextBoardColumn, 0.0f + nextBoardRow);
	gameWindow.draw(revealedTileShape);

	// If the game board tile is a mine tile, draw the mine
	if (inTile == NextToTile::mineTile) 
	{
		mineTileShape.setPosition(0.0f + nextBoardColumn, 0.0f + nextBoardRow);

		gameWindow.draw(mineTileShape);

		// If the player can still play, draw a flag
		if (gameBoard.playerStatus()) 
		{
			gameFlagShape.setPosition(0.0f + nextBoardColumn, 0.0f + nextBoardRow);

			gameWindow.draw(gameFlagShape);
		}
	}

	// If the game board tile that the player clicked on is a number tile, draw the appropriate numbers
	if (inTile == NextToTile::numberTile) 
	{
		// Switch to draw the game board tile numbers
		switch (gameTile.getTileNumber()) 
		{
			// Number 1
		case 1:
			gameDigitOneShape.setPosition(0.0f + nextBoardColumn, 0.0f + nextBoardRow);

			gameWindow.draw(gameDigitOneShape);

			break;
			// Number 2
		case 2:
			gameDigitTwoShape.setPosition(0.0f + nextBoardColumn, 0.0f + nextBoardRow);

			gameWindow.draw(gameDigitTwoShape);

			break;
			// Number 3
		case 3:
			gameDigitThreeShape.setPosition(0.0f + nextBoardColumn, 0.0f + nextBoardRow);

			gameWindow.draw(gameDigitThreeShape);

			break;
			// Number 4
		case 4:
			gameDigitFourShape.setPosition(0.0f + nextBoardColumn, 0.0f + nextBoardRow);

			gameWindow.draw(gameDigitFourShape);

			break;
			// Number 5
		case 5:
			gameDigitFiveShape.setPosition(0.0f + nextBoardColumn, 0.0f + nextBoardRow);

			gameWindow.draw(gameDigitFiveShape);

			break;
			// Number 6
		case 6:
			gameDigitSixShape.setPosition(0.0f + nextBoardColumn, 0.0f + nextBoardRow);

			gameWindow.draw(gameDigitSixShape);

			break;
			// Number 7
		case 7:
			gameDigitSevenShape.setPosition(0.0f + nextBoardColumn, 0.0f + nextBoardRow);

			gameWindow.draw(gameDigitSevenShape);

			break;
			// Number 8
		case 8:
			gameDigitEightShape.setPosition(0.0f + nextBoardColumn, 0.0f + nextBoardRow);

			gameWindow.draw(gameDigitEightShape);

			break;
		}
	}
}

// Function definition for displaying the game board's mine amount in the game window
void displayGameBoardMineCount(GameBoards& gameBoard, sf::RenderWindow& gameWindow, float& nextBoardColumn, unsigned int& boardRow)
{
	int gameBoardMineAmount = gameBoard.flagCount();

	// First digit of the game board's mine amount
	int gameBoardMineAmountFirstDigit = 0;

	// If the game board's mine amount is less than 0, make the first digit a negative sign
	if (gameBoardMineAmount < 0) 
	{
		gameBoardMineAmountFirstDigit = 10;

		gameBoardMineAmount *= -1;
	}
	// If the game board's mine amount is more than 99, make it a 1 to support the 100s
	else if (gameBoardMineAmount > 99)
		gameBoardMineAmountFirstDigit = 1;
	// If the game board's mine amount is more than 199, make it a 2 to support the 200s
	else if (gameBoardMineAmount > 199)
		gameBoardMineAmountFirstDigit = 2;
	// If the game board's mine amount is more than 299, make it a 3 to support the 300s
	else if (gameBoardMineAmount > 299)
		gameBoardMineAmountFirstDigit = 3;
	// If the game board's mine amount is more than 399, make it a 4 to support the 400s
	else if (gameBoardMineAmount > 399)
		gameBoardMineAmountFirstDigit = 4;
	// If the game board's mine amount is more than 499, make it a 5 to support the 500s
	else if (gameBoardMineAmount > 499)
		gameBoardMineAmountFirstDigit = 5;

	// Second digit of the game board's mine amount using modulus + integer division
	int gameBoardMineAmountSecondDigit = (gameBoardMineAmount % 100) / 10;

	// Third digit of the game board's mine amount using modulus
	int gameBoardMineAmountThirdDigit = gameBoardMineAmount % 10;

	// Setting the texture + the position of the first digit + drawing it
	gameDigitShape.setTextureRect(sf::IntRect(21 * gameBoardMineAmountFirstDigit, 0, 21, 32));

	gameDigitShape.setPosition(0.0f + nextBoardColumn, 0.0f + (gameTileSize * (boardRow + 1)));

	gameWindow.draw(gameDigitShape);

	// Setting the texture + the position of the second digit + drawing it
	gameDigitShape.setTextureRect(sf::IntRect(21 * gameBoardMineAmountSecondDigit, 0, 21, 32));

	gameDigitShape.setPosition(0.0f + nextBoardColumn + 21, 0.0f + (gameTileSize * (boardRow + 1)));

	gameWindow.draw(gameDigitShape);

	// Setting the texture + the position of the third digit + drawing it
	gameDigitShape.setTextureRect(sf::IntRect(21 * gameBoardMineAmountThirdDigit, 0, 21, 32));

	gameDigitShape.setPosition(0.0f + nextBoardColumn + 21 * 2, 0.0f + (gameTileSize * (boardRow + 1)));

	gameWindow.draw(gameDigitShape);
}

// Function definition to display the game board's faces into the game window
void displayGameBoardFaces(GameBoards& gameBoard, sf::RenderWindow& gameWindow, float& nextBoardColumn, unsigned int& boardRow)
{
	// If the player's status says that they win, set the position + draw the winning face into the game window
	if (gameBoard.playerStatus()) 
	{
		winningFaceShape.setPosition(0.0f + nextBoardColumn, 0.0f + (gameTileSize * (boardRow + 1)));

		gameWindow.draw(winningFaceShape);
	}
	// Else if the game is still on, set the position + draw the happy face into the game window
	else if (gameBoard.gameOn()) 
	{
		happyFaceShape.setPosition(0.0f + nextBoardColumn, 0.0f + (gameTileSize * (boardRow + 1)));

		gameWindow.draw(happyFaceShape);
	}
	// Else, set the position + draw the losing face into the game window
	else 
	{
		losingFaceShape.setPosition(0.0f + nextBoardColumn, 0.0f + (gameTileSize * (boardRow + 1)));

		gameWindow.draw(losingFaceShape);
	}
}

// Function definition to display the game board's various buttons into the game window
void displayGameBoardButtons(sf::RenderWindow& gameWindow, float& nextBoardColumn, unsigned int& boardRow)
{
	// Setting the position + drawing the toggle debug into the game window
	toggleDebugButtonShape.setPosition(0.0f + nextBoardColumn, 0.0f + (gameTileSize * (boardRow + 1)));

	gameWindow.draw(toggleDebugButtonShape);

	// Setting the position + drawing the test button for testboard1 into the game window
	testOneButtonShape.setPosition(0.0f + nextBoardColumn + gameButtonSize, 0.0f + (gameTileSize * (boardRow + 1)));

	gameWindow.draw(testOneButtonShape);

	// Setting the position + drawing the test button for testboard2 into the game window
	testTwoButtonShape.setPosition(0.0f + nextBoardColumn + 2 * gameButtonSize, 0.0f + (gameTileSize * (boardRow + 1)));

	gameWindow.draw(testTwoButtonShape);

	// Setting the position + drawing the test button for testboard3 into the game window
	testThreeButtonShape.setPosition(0.0f + nextBoardColumn + 3 * gameButtonSize, 0.0f + (gameTileSize * (boardRow + 1)));

	gameWindow.draw(testThreeButtonShape);
}


