#include "PathSearch.h"

namespace ufl_cap4053
{
	namespace searches
	{
		// Default constructor for PathSearch that takes no arguments
		PathSearch::PathSearch()
		{
			// Initializing the starting, current, + ending tiles to nullptr
			thePathsStartingTile = nullptr;

			thePathsCurrentTile = nullptr;

			thePathsEndingTile = nullptr;

			// Initializing the tiles radius to 0
			theTilesRadius = 0;

			// Initializing the search completion flag to false
			theSearchIsCompleted = false;
		}

		// Destructor for PathSearch that cleans things up before deleting the actual obj
		PathSearch::~PathSearch()
		{
			// Setting the starting, current, + ending tiles back to nullptr as we're done w/ them
			thePathsStartingTile = nullptr;

			thePathsCurrentTile = nullptr;

			thePathsEndingTile = nullptr;

			// Using an iterator to go through the search graph from beginning to end
			for (auto & theIterator : theSearchGraph)
			{
				// Clearing the tiles vector of adjacent tiles
				theIterator.second->theAdjacentTiles.clear();

				// Resizing the tiles vector of adjacent tiles back to 0
				theIterator.second->theAdjacentTiles.resize(0);

				// Deleting the actual tile
				delete theIterator.second;
			}

			// While the priority queue isn't empty, keep popping out tiles until it's cleaned up
			while (!thePriorityQueue.empty())
			{
				// Popping the top tile from the priority queue
				thePriorityQueue.pop();
			}

			// Cleaning up + resizing the result path vector back to 0 as we're done w/ it
			theResultPath.clear();

			theResultPath.resize(0);

			// Cleaning up the search graph as we're also done w/ it
			theSearchGraph.clear();
		}

		// Called after the given tile map is loaded, this is the function that creates the search graph
		void PathSearch::load(TileMap* _tileMap)
		{
			// Iterating through the rows of the freshly loaded tile map
			for (int row = 0; row < _tileMap->getRowCount(); row++)
			{
				// Iterating through the columns of the freshly loaded tile map
				for (int column = 0; column < _tileMap->getColumnCount(); column++)
				{
					// If the tile from the tile map has a terrain weight that isn't 0 (0 = impassable, anything besides 0 = passable)
					if (_tileMap->getTile(row, column)->getWeight() != 0)
					{
						// Creating a new path tile from the tile map's tile
						PathTile* freshPathTile = new PathTile(_tileMap->getTile(row, column));

						// Adding the new path tile to the search graph for future use
						theSearchGraph[{row, column}] = freshPathTile;
					}
				}
			}

			// Using the given tile map to calculate the adjacent tiles
			calculateAdjacentTiles(_tileMap);

			// Using the given tile map to get the tile radius (radius of the largest circle that can be circumscribed by a tile)
			theTilesRadius = _tileMap->getTileRadius();
		}

		// Called before any update of the path planner, this is the function that prepares for search to be performed between the tiles
		// This function is always preceded by at least one call to load()
		void PathSearch::initialize(int startRow, int startCol, int goalRow, int goalCol)
		{
			// Setting the paths starting tile using the given starting row + starting column
			thePathsStartingTile = theSearchGraph[{startRow, startCol}];

			// Setting the paths current tile to nullptr as we'll update it during the actual search
			thePathsCurrentTile = nullptr;

			// Setting the paths ending tile using the given goal row + goal column
			thePathsEndingTile = theSearchGraph[{goalRow, goalCol}];

			// Calculating the cost of the path's starting tile
			calculateTileCost(thePathsStartingTile);

			// Setting the tiles previously visited flag to true
			theSearchGraph[{startRow, startCol}]->previouslyVisitedTile = true;

			// Adding the total cost of the paths starting tile + the tile itself to the priority queue
			thePriorityQueue.push({ thePathsStartingTile->theGrandTotal, thePathsStartingTile });

			// Setting the search completion flag to false as we're about to do a search
			theSearchIsCompleted = false;
		}

		// This function allows the path planner to execute for the specified timeslice (in milliseconds)
		// The search should be performed until the time expires/the solution is found
		// This function is always preceded by at least one call to initialize()
		void PathSearch::update(long timeslice)
		{
			// Initializing the current time in milliseconds
			clock_t theCurrentTime = (clock() / (CLOCKS_PER_SEC / 1000));

			// Initializing the timeslice that will be in milliseconds using the specified timeslice
			clock_t millisecondsTimeslice = timeslice;

			// If the update function is not finished
			if (!isDone())
			{
				// If the specified timeslice is greater than 0
				if (timeslice > 0)
				{
					// While the current time does not reach the specified timeslice
					while (theCurrentTime != millisecondsTimeslice)
					{
						// If the search is completed
						if (theSearchIsCompleted == true)
						{
							// Just exit the update function
							return;
						}

						// Else, we can do another search
						aStarSearch();

						// Updating the current time in milliseconds
						theCurrentTime = (clock() / ((CLOCKS_PER_SEC) / 1000));
					}
				}
				// Else, we can just do a single search
				else
				{
					aStarSearch();
				}
			}
		}

		// Called when the current search data is no longer needed. This function should clean up any memory allocated for this search
		// Note that this is NOT the same as the destructor, as the search obj may be reset to perform another search on the same map
		// May be called before initialize() and/or update() in some cases
		void PathSearch::shutdown()
		{
			// Using an iterator to go through the search graph from beginning to end
			for (auto & theIterator : theSearchGraph)
			{
				// Setting the tiles previously visited flag back to false
				theIterator.second->previouslyVisitedTile = false;
			}

			// While the priority queue isn't empty, keep popping out tiles until it's cleaned up
			while (!thePriorityQueue.empty())
			{
				// Popping the top tile from the priority queue
				thePriorityQueue.pop();
			}
		}

		// Called when the tile map is unloaded. This function should clean up any memory allocated for the tile map
		// This is NOT the same as the destructor, as the search obj may be reinitialized with a new map
		// May be called before load()
		void PathSearch::unload()
		{
			// Using an iterator to go through the search graph from beginning to end
			for (auto & theIterator : theSearchGraph)
			{
				// Clearing the tiles vector of adjacent tiles
				theIterator.second->theAdjacentTiles.clear();

				// Resizing the tiles vector of adjacent tiles back to 0
				theIterator.second->theAdjacentTiles.resize(0);

				// Deleting the actual tile
				delete theIterator.second;
			}

			// While the priority queue isn't empty, keep popping out tiles until it's cleaned up
			while (!thePriorityQueue.empty())
			{
				// Popping the top tile from the priority queue
				thePriorityQueue.pop();
			}

			// Cleaning up + resizing the result path vector back to 0 as we're done w/ it
			theResultPath.clear();

			theResultPath.resize(0);

			// Cleaning up the search graph as we're also done w/ it
			theSearchGraph.clear();

			// Resetting the search completion flag
			theSearchIsCompleted = false;
		}

		// This function returns true if the update function has finished b/c it found a solution, + false otherwise
		// Once a search has completed, this function should continue to return true until initialize() is called
		bool PathSearch::isDone() const
		{
			// If our search is completed + we found a solution
			if (theSearchIsCompleted)
			{
				// Returning true as our search is completed + we found a solution
				return true;
			}

			// Else, returning false as our search is not completed + we don't have a solution
			return false;
		}

		// This function returns a vector containing the solution path as an ordered series of Tile pointers from finish to start
		// Once a search has been completed, this function sould continue to return the path until initialize() is called
		std::vector<Tile const*> const PathSearch::getSolution() const
		{
			// Retrieving the paths ending tile
			PathTile* aPathTile = thePathsEndingTile;
			
			// While the current paths ending tile has not reached the result paths ending tile
			while (aPathTile->theSpecificTileMapPoint != theResultPath.back())
			{
				// Adding a red line from the current paths ending tile to its parent tile
				aPathTile->theSpecificTileMapPoint->addLineTo(aPathTile->theParentTile->theSpecificTileMapPoint, 4290445312);

				// Setting the current paths ending tile to be its parent tile
				aPathTile = aPathTile->theParentTile;
			}

			// When the current paths ending tile has reached the result paths ending tile, we can return the result path itself
			return theResultPath;
		}

		// The actual search
		void PathSearch::aStarSearch()
		{
			// If a solution has not been found + the priority queue still has tiles in it
			if (!thePriorityQueue.empty() || !isDone())
			{
				// Retrieving the paths current tile
				thePathsCurrentTile = thePriorityQueue.top().second;

				// Popping the top tile in the priority queue after we get it
				thePriorityQueue.pop();

				// If the paths current tile is equal to the paths ending tile
				if (thePathsCurrentTile == thePathsEndingTile) // if a solution is found, we're done.
				{
					// Then we have a solution + the search is completed
					theSearchIsCompleted = true;

					// Retrieving the paths ending tile
					PathTile* aPathTile = thePathsEndingTile;

					// If the result path exists
					if (theResultPath.size() != 0)
					{
						// We can just return it
						return;
					}

					// While the paths ending tile is not nullptr
					while (aPathTile != nullptr)
					{
						// Keep adding the tile to the result path
						theResultPath.push_back(aPathTile->theSpecificTileMapPoint);

						// Setting the paths ending tile to the parent tile
						aPathTile = aPathTile->theParentTile;
					}

					// Returning as we have the result path now
					return;
				}

				// Iterating through the paths current tiles adjacent tiles
				for (auto & theAdjacentTile : thePathsCurrentTile->theAdjacentTiles)
				{
					// Retrieving the paths current tile adjacent tile
					PathTile* aPathTile = theAdjacentTile;

					// Calculating the adjacent tiles cost using its given cost + the product of its terrain weight + tile radius doubled
					double aPathTileCost = thePathsCurrentTile->theGivenCost + (theAdjacentTile->theSpecificTileMapPoint->getWeight() * (theTilesRadius * 2));
					
					// If the adjacent tile is a previously visited tile
					if (aPathTile->previouslyVisitedTile == true)
					{
						// If its cost is less than its given cost
						if (aPathTileCost < aPathTile->theGivenCost)
						{
							// Setting the adjacent tiles given cost to be the new cost
							aPathTile->theGivenCost = aPathTileCost;

							// Calculating the adjacent tiles total cost using its given cost + its heuristic cost + weight
							aPathTile->theGrandTotal = aPathTile->theGivenCost + (aPathTile->theHeuristicCost * 1.2);

							// Setting the adjacent tiles parent tile to be the paths current tile
							aPathTile->theParentTile = thePathsCurrentTile;

							// Adding the adjacent tile + its total cost to the priority queue for future use
							thePriorityQueue.push({aPathTile->theGrandTotal, aPathTile});
						}
					}
					// Else, if we have not previously visited the adjacent tile
					else
					{
						// Retrieving the paths ending tiles x-coordinate
						double thePathsEndingTileXCoordinate = thePathsEndingTile->theSpecificTileMapPoint->getXCoordinate();

						// Retrieving the adjacent tiles x-coordinate
						double aPathTileXCoordinate = aPathTile->theSpecificTileMapPoint->getXCoordinate();

						// Retrieving the paths ending tiles y-coordinate
						double thePathsEndingTileYCoordinate = thePathsEndingTile->theSpecificTileMapPoint->getYCoordinate();

						// Retrieving the adjacent tiles y-coordinate
						double aPathTileYCoordinate = aPathTile->theSpecificTileMapPoint->getYCoordinate();
						
						// Calculating the heuristic cost using the square root of the coordinates squared
						double aHeuristicCost = sqrt(((thePathsEndingTileXCoordinate - aPathTileXCoordinate) * (thePathsEndingTileXCoordinate - aPathTileXCoordinate)) + ((thePathsEndingTileYCoordinate - aPathTileYCoordinate) * (thePathsEndingTileYCoordinate - aPathTileYCoordinate)));

						// Setting the adjacent tiles given cost to be the new cost
						aPathTile->theGivenCost = aPathTileCost;

						// Setting the adjacent tiles heuristic cost to be the newly calculated heuristic cost
						aPathTile->theHeuristicCost = aHeuristicCost;

						// Calculating the adjacent tiles total cost using its given cost + its heuristic cost + weight
						aPathTile->theGrandTotal = aPathTile->theGivenCost + (aPathTile->theHeuristicCost * 1.2);

						// Setting the adjacent tiles parent tile to be the paths current tile
						aPathTile->theParentTile = thePathsCurrentTile;

						// The adjacent tile is now a visited tile
						aPathTile->previouslyVisitedTile = true;

						// Adding the adjacent tile + its total cost to the priority queue for future use
						thePriorityQueue.push({aPathTile->theGrandTotal, aPathTile});

						// If the adjacent tile is not equal to the paths ending tile
						if (aPathTile != thePathsEndingTile)
						{
							// Adding a green marker to the adjacent tile we visited
							aPathTile->theSpecificTileMapPoint->setMarker(4278240256);
						}
					}
				}
			}
		}

		// Function for determining the adjacency of Tile objs to simplify IDing of adjacent tiles
		bool PathSearch::isAdjacent(const Tile* theLeftHandSide, const Tile* theRightHandSide)
		{
			// Retrieving the left hand sides row
			int theLeftHandSideRow = theLeftHandSide->getRow();

			// Retrieving the right hand sides row
			int theRightHandSideRow = theRightHandSide->getRow();

			// Calculating the distance between their two rows
			int theDistanceBetweenRows = theRightHandSideRow - theLeftHandSideRow;

			// Retrieving the left hand sides column
			int theLeftHandSideColumn = theLeftHandSide->getColumn();

			// Retrieving the right hand sides column
			int theRightHandSideColumn = theRightHandSide->getColumn();

			// Calculating the distance between their two columns
			int theDistanceBetweenColumns = theRightHandSideColumn - theLeftHandSideColumn;

			// If the distance between them is less than or equal to 1
			if (theDistanceBetweenRows <= 1 && theDistanceBetweenColumns <= 1)
			{
				// If the left hand sides row is odd
				if (theLeftHandSide->getRow() % 2 == 1)
				{
					// If the distance between them is -1
					if (theDistanceBetweenRows == -1 && theDistanceBetweenColumns == -1)
					{
						// Returning false as the tiles are not adjacent
						return false;
					}

					// If the distance between rows is 1 + the distance between columns is -1
					if (theDistanceBetweenRows == 1 && theDistanceBetweenColumns == -1)
					{
						// Returning false as the tiles are not adjacent
						return false;
					}

				}
				// Else, the row is even
				else
				{
					// If the distance between rows is -1 + the distance between columns is 1
					if (theDistanceBetweenRows == -1 && theDistanceBetweenColumns == 1 )
					{
						// Returning false as the tiles are not adjacent
						return false;
					}

					// If the distance between rows is 1 + the distance between columns is 1
					if (theDistanceBetweenRows == 1 && theDistanceBetweenColumns == 1)
					{
						// Returning false as the tiles are not adjacent
						return false;
					}
				}

				// Returning true as the tiles are adjacent
				return true;
			}

			// Returning false if the distance between them is not less than or equal to 1, making the tiles not adjacent
			return false;
		}

		// Function for determining the adjacency of Tile objs to simplify IDing of adjacent tiles
		void PathSearch::calculateAdjacentTiles(TileMap* theTileMap)
		{
			// Using an iterator to go through the search graph from beginning to end
			for (auto & theIterator : theSearchGraph)
			{
				// Iterating through the search graphs x-coordinates
				for (int i = -1; i <= 1; i++)
				{
					// Iterating through the search graphs y-coordinates
					for (int j = -1; j <= 1; j++)
					{
						// If the tiles terrain weight is not equal to 0 (0 = impassable, anything besides 0 = passable) + this tile is not the first tile + the tiles row does not go past the tile maps upper row bound + the tiles column does not go past the tile maps upper column bound
						if ((theIterator.first.first + i < theTileMap->getRowCount()) && (theIterator.first.second + j < theTileMap->getColumnCount()) && ((theIterator.first.first + i >= 0) && (theIterator.first.second + j >= 0)) && (theTileMap->getTile(theIterator.first.first + i, theIterator.first.second + j)->getWeight() != 0))
						{
							// If the tiles row is not equal to the tile maps upper row bound + if the tiles column is not equal to the tile maps upper column bound
							if (theIterator.first.first + i != theIterator.first.first || theIterator.first.second + j != theIterator.first.second)
							{
								// If the two tiles are adjacent to each other
								if (isAdjacent(theSearchGraph[{theIterator.first.first, theIterator.first.second}]->theSpecificTileMapPoint, theTileMap->getTile(theIterator.first.first + i, theIterator.first.second + j)))
								{
									// Adding the tile to the other tiles vector of tiles that are adjacent to it
									theSearchGraph[{theIterator.first.first, theIterator.first.second}]->theAdjacentTiles.push_back(theSearchGraph[{theIterator.first.first + i, theIterator.first.second + j}]);
								}
							}
						}
					}
				}
			}
		}

		// Function for calculating the cost to the given tile
		void PathSearch::calculateTileCost(PathTile* theGivenTile)
		{
			// Retrieving the paths ending tiles x-coordinate
			double thePathsEndingTilesXCoordinate = thePathsEndingTile->theSpecificTileMapPoint->getXCoordinate();

			// Retrieving the given tiles x-coordinate
			double theGivenTilesXCoordinate = theGivenTile->theSpecificTileMapPoint->getXCoordinate();

			// Retrieving the paths ending tiles y-coordinate
			double thePathsEndingTileYCoordinate = thePathsEndingTile->theSpecificTileMapPoint->getYCoordinate();

			// Retrieving the given tiles y-coordinate
			double theGivenTilesYCoordinate = theGivenTile->theSpecificTileMapPoint->getYCoordinate();

			// Calculating the heuristic costs x-coordinate
			double heuristicXCoordinate = abs(2 * (thePathsEndingTilesXCoordinate - theGivenTilesXCoordinate));

			// Calculating the heuristic costs y-coordinate
			double heuristicYCoordinate = abs(2 * (thePathsEndingTileYCoordinate - theGivenTilesYCoordinate));

			// Calculating the given tiles heuristic cost using the two x-coordinates + the two y-coordinates
			theGivenTile->theHeuristicCost = sqrt(heuristicXCoordinate + heuristicYCoordinate);

			// If the given tile is not equal to the paths starting tile, calculate the given tiles given cost
			if (theGivenTile != thePathsStartingTile)
			{
				// Calculating the given tiles given cost using its terrain weight, the tiles radius doubled, + the paths current tiles given cost
				theGivenTile->theGivenCost = theGivenTile->theSpecificTileMapPoint->getWeight() * (theTilesRadius * 2) + thePathsCurrentTile->theGivenCost;
			}

			// Calculating the given tiles total cost to be the sum of its given cost + its heuristic cost
			theGivenTile->theGrandTotal = theGivenTile->theGivenCost + theGivenTile->theHeuristicCost;
		}
	}
}
