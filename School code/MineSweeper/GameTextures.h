#pragma once

#include <SFML/Graphics.hpp>

// Size of the game tile
float gameTileSize = 32.0f;

// Size of the game button
float gameButtonSize = 64.0f;

// Shape of a mine tile
sf::RectangleShape mineTileShape(sf::Vector2f(gameTileSize, gameTileSize));

// Texture of a mine tile
sf::Texture mineTileTexture;

// Shape of a hidden tile
sf::RectangleShape hiddenTileShape(sf::Vector2f(gameTileSize, gameTileSize));

// Texture of a hidden tile
sf::Texture hiddenTileTexture;

// Shape of a revealed tile
sf::RectangleShape revealedTileShape(sf::Vector2f(gameTileSize, gameTileSize));

// Texture of a revealed tile
sf::Texture revealedTileTexture;

// Shape of a digit 1
sf::RectangleShape gameDigitOneShape(sf::Vector2f(gameTileSize, gameTileSize));

// Texture of a digit 1
sf::Texture gameDigitOneTexture;

// Shape of a digit 2
sf::RectangleShape gameDigitTwoShape(sf::Vector2f(gameTileSize, gameTileSize));

// Texture of a digit 2
sf::Texture gameDigitTwoTexture;

// Shape of a digit 3
sf::RectangleShape gameDigitThreeShape(sf::Vector2f(gameTileSize, gameTileSize));

// Texture of a digit 3
sf::Texture gameDigitThreeTexture;

// Shape of a digit 4
sf::RectangleShape gameDigitFourShape(sf::Vector2f(gameTileSize, gameTileSize));

// Texture of a digit 4
sf::Texture gameDigitFourTexture;

// Shape of a digit 5
sf::RectangleShape gameDigitFiveShape(sf::Vector2f(gameTileSize, gameTileSize));

// Texture of a digit 5
sf::Texture gameDigitFiveTexture;

// Shape of a digit 6
sf::RectangleShape gameDigitSixShape(sf::Vector2f(gameTileSize, gameTileSize));

// Texture of a digit 6
sf::Texture gameDigitSixTexture;

// Shape of a digit 7
sf::RectangleShape gameDigitSevenShape(sf::Vector2f(gameTileSize, gameTileSize));

// Texture of a digit 7
sf::Texture gameDigitSevenTexture;

// Shape of a digit 8
sf::RectangleShape gameDigitEightShape(sf::Vector2f(gameTileSize, gameTileSize));

// Texture of a digit 8
sf::Texture gameDigitEightTexture;

// Shape of a game flag
sf::RectangleShape gameFlagShape(sf::Vector2f(gameTileSize, gameTileSize));

// Texture of a game flag
sf::Texture gameFlagTexture;

// Shape of the happy face
sf::RectangleShape happyFaceShape(sf::Vector2f(gameButtonSize, gameButtonSize));

// Texture of the happy face
sf::Texture happyFaceTexture;

// Shape of the winning face
sf::RectangleShape winningFaceShape(sf::Vector2f(gameButtonSize, gameButtonSize));

// Texture of the winning face
sf::Texture winningFaceTexture;

// Shape of the losing face
sf::RectangleShape losingFaceShape(sf::Vector2f(gameButtonSize, gameButtonSize));

// Texture of the losing face
sf::Texture losingFaceTexture;

// Shape of a digit
sf::RectangleShape gameDigitShape(sf::Vector2f(21, gameTileSize));

// Texture of a digit
sf::Texture gameDigitTexture;

// Shape of the toggle debug button
sf::RectangleShape toggleDebugButtonShape(sf::Vector2f(gameButtonSize, gameButtonSize));

// Texture of the toggle debug button
sf::Texture toggleDebugButtonTexture;

// Shape of the Test #1 button
sf::RectangleShape testOneButtonShape(sf::Vector2f(gameButtonSize, gameButtonSize));

// Texture of the Test #1 button
sf::Texture testOneButtonTexture;

// Shape of the Test #2 button
sf::RectangleShape testTwoButtonShape(sf::Vector2f(gameButtonSize, gameButtonSize));

// Texture of the Test #2 button
sf::Texture testTwoButtonTexture;

// Shape of the Test #3 button
sf::RectangleShape testThreeButtonShape(sf::Vector2f(gameButtonSize, gameButtonSize));

// Texture of the Test #3 button
sf::Texture testThreeButtonTexture;