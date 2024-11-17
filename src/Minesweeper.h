#pragma once

#include <queue>
#include <string>
#include <vector>
#include <array>

#ifdef __EMSCRIPTEN__
#include <emscripten/bind.h>
#endif

namespace Minesweeper
{
	static const int TILE_UP_OFFSET = 10, FLAG_OFFSET = 20, BOMB = 9;

	struct Position
	{
		int x;
		int y;
	};

	struct Tile
	{
		Position position;
		int value;
	};

#ifdef __EMSCRIPTEN__
	typedef emscripten::val SweptTiles;
#else
	typedef std::vector<Tile> SweptTiles;
#endif

	/**
	 * The `MineField` class manages the state of the minefield, including the grid, mines, flags,
	 * and game logic for sweeping and flagging tiles. It provides functions for interacting with
	 * the field, revealing the grid, and converting the grid state to a human-readable string.
	 */
	class MineField
	{
	public:
		MineField(int width, int height);

		MineField(int width, int height, int num_of_mines, int seed = 0, int spawnGuardRadius = 1);

		~MineField();

		SweptTiles sweep(int x, int y);

		int flag(int x, int y);

		void clear();

		void revealGrid();

		std::string &toString(std::string &buffer) const;

		std::string toString() const;

		int getWidth() const;

		int getHeight() const;

		const std::vector<int> &getGrid() const;

		int getSeed() const;

		int getAt(int x, int y) const;

		void setAt(int x, int y, int value);

	private:
		typedef std::array<Tile, 8> NearbyTiles;

		static constexpr char symbols[11] = {' ', '1', '2', '3', '4', '5', '6', '7', '8', 'x', 'o'};

		std::vector<int> grid;
		std::queue<Position> sweepQueue;
		int width;
		int height;
		int seed;
		int mineCount;
		int spawnGuardRadius;
		bool firstSweep;

#ifdef __EMSCRIPTEN__
		SweptTiles createSweptTiles()
		{
			return emscripten::val::array();
		}
		void push_back(SweptTiles &sweptTiles, const Tile &tile)
		{
			sweptTiles.call<void>("push", tile);
		}
#else
		SweptTiles createSweptTiles()
		{
			return SweptTiles();
		}
		void push_back(SweptTiles &sweptTiles, const Tile &tile)
		{
			sweptTiles.push_back(tile);
		}
#endif

		void populateGrid(int first_x, int first_y);

		int getNearbyBombs(int x, int y) const;

		int getNearby(int x, int y, NearbyTiles &nearby) const;

		inline bool isFlagged(int tile_value) const;

		inline bool isTileUp(int tile_value) const;

		inline bool isBomb(int tile_value) const;

		inline bool isValidTile(int x, int y) const;
	};

} // namespace Minesweeper