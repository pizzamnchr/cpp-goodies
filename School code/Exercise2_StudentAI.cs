using GameAI.GamePlaying.Core;

using System.Collections.Generic;

namespace GameAI.GamePlaying
{
    public class StudentAI : Behavior
    {
        // Implementation of Minimax. Determines the best move for the player specified by color on the given
        // board, looking ahead the number of steps indicated by lookAheadDepth. This method will not be called
        // unless there is at least one possible move for the player specified by color.
        public ComputerMove Run(int color, Board board, int lookAheadDepth)
        {
            return ReversiMinimax(int.MinValue, int.MaxValue, color, board, lookAheadDepth);
        }

        // Returns a strategic value estimate for board based on position and color of pieces. It is recommended that
        // students implement a method with this functionality to evaluate board states at the lookahead depth cutoff.
        private int Evaluate(Board board)
        {
            // Initializing the board score 
            int boardScore = 0;

            // Iterating through the rows of the game board
            for (int boardRow = 0; boardRow < Board.Height; boardRow++)
            {
                // Iterating through the columns of the game board
                for (int boardColumn = 0; boardColumn < Board.Width; boardColumn++)
                {
                    // If the board square is in a corner
                    if ((boardColumn % (Board.Width - 1) == 0) && (boardRow % (Board.Height - 1) == 0))
                    {
                        // Multiplying the board square's value by 100 if it's in a corner
                        boardScore += 100 * board.GetTile(boardRow, boardColumn);
                    }
                    // If a board square is not in a corner but is on the side of the board
                    else if ((boardColumn % (Board.Width - 1) == 0) || (boardRow % (Board.Height - 1) == 0))
                    {
                        // Multiplying the board square's value by 10 if it's not in a corner but is on the side of the board
                        boardScore += 10 * board.GetTile(boardRow, boardColumn);
                    }
                    // If the value for each board square is identified
                    else
                    {
                        // Adding all the board square values together if their values are known
                        boardScore += board.GetTile(boardRow, boardColumn);
                    }
                }
            }

            // If there are no valid moves for either player (the game is over)
            if (board.IsTerminalState())
            {
                // If the board score is greater than 0, set it to 10000, else, set it to -10000
                boardScore += boardScore > 0 ? 10000 : -10000;
            }

            // Returning the board score
            return boardScore;
        }

        // The actual implementation of Minimax. Determines the best move for the player specified by color on the given
        // board, looking ahead the number of steps indicated by lookAheadDepth. This method will not be called
        // unless there is at least one possible move for the player specified by color.
        private ComputerMove ReversiMinimax(int minimum, int maximum, int color, Board board, int lookAheadDepth)
        {
            // Initializing a fresh board object
            Board freshBoard = new Board();

            // Initializing the best move for the player that we'll return
            ComputerMove playerBestMove = null;

            // Initializing a list of computer moves that will store all of the possible moves for the player
            List<ComputerMove> possibleMoves = new List<ComputerMove>();

            // Iterating through the rows of the game board
            for (int boardRow = 0; boardRow < Board.Height; boardRow++)
            {
                // Iterating through the columns of the game board
                for (int boardColumn = 0; boardColumn < Board.Width; boardColumn++)
                {
                    // If the specific move is possible for the specified player color
                    if (board.IsValidMove(color, boardRow, boardColumn))
                    {
                        // Add it to our list of possible moves for the player
                        possibleMoves.Add(new ComputerMove(boardRow, boardColumn));
                    }
                }
            }

            // Iterating through all of the possible moves for the player
            foreach (ComputerMove possibleMove in possibleMoves)
            {
                // Setting the fresh board as a copy of the board
                freshBoard.Copy(board);

                // Placing a disc for the player on the board + flips any outflanked opponents
                // Does NOT check that the move is valid
                freshBoard.MakeMove(color, possibleMove.row, possibleMove.column);

                // If the look ahead depth is equal to 0 / there are no valid moves for either player (the game is over)
                if (lookAheadDepth == 0 || freshBoard.IsTerminalState())
                {
                    // Using the strategic value estimate for board based on position and color of pieces to get the possible move's rank
                    possibleMove.rank = Evaluate(freshBoard);
                }
                // Else, if the look ahead depth isn't equal to 0 / there are valid moves for either player
                else
                {
                    // Using the rank of the best move for the player specified by color on the given board to get the possible move's rank
                    possibleMove.rank = ReversiMinimax(minimum, maximum, (freshBoard.HasAnyValidMove(color * -1) ? color * -1 : color), freshBoard, lookAheadDepth - 1).rank;
                }

                // If the possible move's rank is better than the current best move for the player / the best move happens to be null
                if ((playerBestMove == null) || (possibleMove.rank * color > playerBestMove.rank * color))
                {
                    // Updating the current best move for the player to the better move
                    playerBestMove = possibleMove;

                    // If the player's current best move's rank is greater than the minimum + the player's color is white
                    if ((playerBestMove.rank > minimum) && (color == 1))
                    {
                        // Updating the minimum to be the player's current best move's rank
                        minimum = playerBestMove.rank;
                    }
                    // Else, if the player's current best move's rank is less than the maximum + the player's color is black
                    else if ((playerBestMove.rank < maximum) && (color == -1))
                    {
                        // Updating the maximum to be the player's current best move's rank
                        maximum = playerBestMove.rank;
                    }

                    // If the minimum is less than / equal to the maximum
                    if (minimum >= maximum)
                    {
                        // We're done w/ this move
                        break;
                    }
                }
            }

            // Returning the player's current best move
            return playerBestMove;
        } 
    }
}