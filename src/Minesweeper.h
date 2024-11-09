#pragma once

#include <queue>
#include <string>
#include <vector>
#include <array>

namespace Minesweeper
{
	static const int TILE_UP_OFFSET = 10, FLAG_OFFSET = 20, BOMB = 9;

	struct Position
	{
		int x, y;
	};

	struct Tile
	{
		Position position;
		int value;
	};

	class MineField
	{
	public:
		MineField(int width, int height);

		MineField(int width, int height, int num_of_mines, int seed = 0, int spawnGuardRadius = 1);

		~MineField();

		// left click a tile
		int sweep(int x, int y);

		// right click a tile
		int flag(int x, int y);

		// set all tiles to empty
		void clear();

		void revealGrid();

		std::string& toString(std::string& buffer) const;
		std::string toString() const;

		// if you want to change the dimensions, create a new MineField.
		int getWidth() const;

		int getHeight() const;

		const std::vector<int>& getGrid() const;

		int getSeed() const;

	private:
		typedef std::array<Tile, 8> TilesArray;

		static constexpr char symbols[11] = { ' ', '1', '2', '3', '4', '5', '6', '7', '8', 'x' , 'o' };

		std::vector<int> grid;
		std::queue<Position> sweepQueue;
		int width;
		int height;
		int seed;
		int mineCount;
		int spawnGuardRadius;
		bool firstSweep;

		void populateGrid(int first_x, int first_y);

		int getNearbyBombs(int x, int y) const;

		int getNearby(int x, int y, TilesArray& nearby) const;

		inline bool isFlagged(int tile_value) const;

		inline bool isTileUp(int tile_value) const;

		inline bool isBomb(int tile_value) const;

		inline bool isValidTile(int x, int y) const;

		inline int getAt(int x, int y) const;

		inline void setAt(int x, int y, int value);
	};
};