#include "minesweeper.h"
#include <stdexcept>
#include <iostream>
using namespace Minesweeper;

#ifdef __EMSCRIPTEN__
using namespace emscripten;
// std::shared_ptr<Minesweeper::MineField> createMyClass(int width, int height) {
// 	return std::make_shared<Minesweeper::MineField>(width, height);
// }

// Binding code
EMSCRIPTEN_BINDINGS(minefield)
{
	value_object<Position>("Position")
		.field("x", &Position::x)
		.field("y", &Position::y);

	value_object<Tile>("Tile")
		.field("position", &Tile::position)
		.field("value", &Tile::value);

	class_<Minesweeper::MineField>("MineField")
		.smart_ptr<std::shared_ptr<Minesweeper::MineField>>("shared_ptr<Minesweeper::MineField>")
		//.smart_ptr_constructor("MineField(width, height)", &std::make_shared<Minesweeper::MineField, int, int>)
		//.smart_ptr_constructor("MineField(width, height, bombCount, seed, spawnGuardRadius)", &std::make_shared<Minesweeper::MineField, int, int, int, int, int>)

		.constructor(&std::make_shared<Minesweeper::MineField, int, int>)
		.constructor(&std::make_shared<Minesweeper::MineField, int, int, int, int, int>)

		//.constructor<int, int>("MineField(width, height)")
		//.constructor<int, int, int, int, int>()
		.function("sweep(x, y)", &Minesweeper::MineField::sweep)
		.function("flag(x, y)", &Minesweeper::MineField::flag)
		.function("clear", &Minesweeper::MineField::clear)
		.function("revealGrid", &Minesweeper::MineField::revealGrid)
		.function("getWidth", &Minesweeper::MineField::getWidth)
		.function("getHeight", &Minesweeper::MineField::getHeight)
		.function("getSeed", &Minesweeper::MineField::getSeed)
		.function("getAt(x, y)", &Minesweeper::MineField::getAt)
		.function("setAt(x, y)", &Minesweeper::MineField::setAt)
		.function("toString", select_overload<std::string() const>(&Minesweeper::MineField::toString));

	// function("createMyClass", createMyClass);
}
#endif

MineField::MineField(int width, int height) : MineField(width, height, (int)(width * height * .1)) {}

Minesweeper::MineField::MineField(int width, int height, int mineCount, int seed, int spawnGuardRadius)
{
	if (spawnGuardRadius - 1 > width || spawnGuardRadius - 1 > height)
	{
		throw std::invalid_argument("spawn_guard_radius greater than grid dimenisons");
	}

	this->width = width;
	this->height = height;
	this->mineCount = mineCount;
	this->seed = seed;
	this->spawnGuardRadius = spawnGuardRadius;
	this->firstSweep = true;

	clear();
}

MineField::~MineField()
{
}

SweptTiles MineField::sweep(int x, int y)
{
	if (x < 0 || x >= width)
	{
		throw std::out_of_range("sweep x position out-of-bounds");
	}

	if (y < 0 || y >= height)
	{
		throw std::out_of_range("sweep x position out-of-bounds");
	}

	SweptTiles swept_tiles = createSweptTiles();

	if (firstSweep)
	{
		firstSweep = false;
		populateGrid(x, y);
	}

	int tile_value = getAt(x, y);
	;

	// sweep shortcut
	// if (tile_value >=0 && tile_value <=9) {
	// 	NearbyTiles nearby;
	// 	int count = getNearby(x, y, nearby);

	// 	for (size_t i = 0; i < count; ++i) {
	// 		Tile& tile = nearby[i];
	// 		if (!isFlagged(tile.value)) {
	// 			sweepQueue.push(tile.position);
	// 		}
	// 	}
	// }
	// else {
	// 	sweepQueue.push(Position{ x, y });
	// }

	sweepQueue.push(Position{x, y});

	while (!sweepQueue.empty())
	{
		Position curr_Position = sweepQueue.front();
		sweepQueue.pop();

		int &curr_x = curr_Position.x;
		int &curr_y = curr_Position.y;

		tile_value = getAt(curr_x, curr_y);

		if (tile_value == 0)
			continue;

		// if tile is up and not flagged
		if (isTileUp(tile_value) && !isFlagged(tile_value))
		{
			// reveal tile
			tile_value = tile_value - TILE_UP_OFFSET;
			setAt(curr_x, curr_y, tile_value);

			// add tile to swept list.
			push_back(swept_tiles, Tile{curr_Position, tile_value});

			// check if it was bomb, if so then reveal grid
			if (isBomb(tile_value))
			{
				revealGrid();
				return swept_tiles;
			}

			// if tile has no bombs nearby, sweep nearby tiles
			if (tile_value == 0)
			{
				if (curr_y + 1 < height)
				{
					if (curr_x + 1 < width)
						sweepQueue.push(Position{curr_x + 1, curr_y + 1});
					sweepQueue.push(Position{curr_x, curr_y + 1});
					if (curr_x - 1 >= 0)
						sweepQueue.push(Position{curr_x - 1, curr_y + 1});
				}

				if (curr_x + 1 < width)
					sweepQueue.push(Position{curr_x + 1, curr_y});
				if (curr_x - 1 >= 0)
					sweepQueue.push(Position{curr_x - 1, curr_y});

				if (curr_y - 1 >= 0)
				{
					if (curr_x + 1 < width)
						sweepQueue.push(Position{curr_x + 1, curr_y - 1});
					sweepQueue.push(Position{curr_x, curr_y - 1});
					if (curr_x - 1 >= 0)
						sweepQueue.push(Position{curr_x - 1, curr_y - 1});
				}
			}
		}
	}

	return swept_tiles;
}

int MineField::flag(int x, int y)
{
	int tile_value = getAt(x, y);

	if (isTileUp(tile_value))
	{
		if (isFlagged(tile_value))
		{
			// remove flag from tile
			tile_value = tile_value - TILE_UP_OFFSET;
		}
		else
		{
			// add flag to tile
			tile_value = tile_value + TILE_UP_OFFSET;
		}
	}
	setAt(x, y, tile_value);

	return tile_value;
}

void MineField::populateGrid(int first_x, int first_y)
{
	// declare 2-d vector of Position of tiles that have no bombs
	std::vector<Position> empty_tiles;
	empty_tiles.reserve(width * height);

	// fill 2-d vector with all grid Position
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			empty_tiles.push_back({x, y});
		}
	}

	// remove spawn tiles from vector so bombs can't spawn on them
	for (int y = first_y + spawnGuardRadius; y >= first_y - spawnGuardRadius; --y)
	{
		int begin = first_x + width * y - spawnGuardRadius;	  // inclusive
		int end = first_x + width * y + spawnGuardRadius + 1; // exclusive
		empty_tiles.erase(empty_tiles.begin() + begin, empty_tiles.begin() + end);
	}

	// randomly populate grid with bombs
	srand(seed);
	for (int i = 0; i < mineCount && !empty_tiles.empty(); ++i)
	{
		int idx = rand() % empty_tiles.size();
		Position pos = empty_tiles[idx];
		setAt(pos.x, pos.y, BOMB + TILE_UP_OFFSET);
		empty_tiles.erase(empty_tiles.begin() + idx);
	}

	// set number tiles
	for (int x = 0; x < width; ++x)
	{
		for (int y = 0; y < height; ++y)
		{
			if (!isBomb(getAt(x, y)))
			{
				setAt(x, y, getNearbyBombs(x, y) + TILE_UP_OFFSET);
			}
		}
	}
}

int MineField::getNearbyBombs(int x, int y) const
{
	int nearby_bombs = 0;
	NearbyTiles nearby;
	int count = getNearby(x, y, nearby);

	for (size_t i = 0; i < count; ++i)
	{
		Tile &tile = nearby[i];
		if (isBomb(tile.value))
		{
			++nearby_bombs;
		}
	}

	return nearby_bombs;
}

int MineField::getNearby(int x, int y, NearbyTiles &nearby) const
{
	int count = 0;
	if (y + 1 < height)
	{
		if (x + 1 < width)
		{
			nearby[count++] = {Position{x + 1, y + 1}, getAt(x + 1, y + 1)};
		}

		nearby[count++] = {Position{x, y + 1}, getAt(x, y + 1)};

		if (x - 1 >= 0)
		{
			nearby[count++] = {Position{x - 1, y + 1}, getAt(x - 1, y + 1)};
		}
	}

	if (x + 1 < width)
	{
		nearby[count++] = {Position{x + 1, y}, getAt(x + 1, y)};
	}
	if (x - 1 >= 0)
	{
		nearby[count++] = {Position{x - 1, y}, getAt(x - 1, y)};
	}

	if (y - 1 >= 0)
	{
		if (x + 1 < width)
		{
			nearby[count++] = {Position{x + 1, y - 1}, getAt(x + 1, y - 1)};
		}

		nearby[count++] = {Position{x, y - 1}, getAt(x, y - 1)};

		if (x - 1 >= 0)
		{
			nearby[count++] = {Position{x - 1, y - 1}, getAt(x - 1, y - 1)};
		}
	}

	return count;
}

std::string &MineField::toString(std::string &buffer) const
{
	int char_width = (width * 2 + 1);
	int stringLength = char_width * height;
	buffer.clear();
	buffer.reserve(stringLength);

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			int tile_value = getAt(x, y);
			char c = 'o';
			if (!isTileUp(tile_value))
				c = symbols[tile_value];
			else if (isFlagged(tile_value))
				c = '>';
			int idx = x * 2 + y * (char_width);
			// strGrid[idx] = c;
			// strGrid[idx + 1] = ' ';
			buffer.push_back(c);
			buffer.push_back(' ');
		}
		buffer.push_back('\n');
		// strGrid[char_width - 1 + y * (char_width)] = '\n';
	}

	return buffer;
}

std::string MineField::toString() const
{
	std::string str;
	return toString(str);
}

void MineField::clear()
{
	grid.assign(width * height, TILE_UP_OFFSET);
	firstSweep = true;
}

void MineField::revealGrid()
{
	for (int x = 0; x < width; ++x)
	{
		for (int y = 0; y < height; ++y)
		{
			int tile_value = getAt(x, y);
			if (isFlagged(tile_value))
				tile_value = tile_value - TILE_UP_OFFSET;
			if (isTileUp(tile_value))
				tile_value = tile_value - TILE_UP_OFFSET;

			setAt(x, y, tile_value);
		}
	}
}

bool MineField::isFlagged(int tile_value) const
{
	return tile_value >= FLAG_OFFSET;
}

bool MineField::isTileUp(int tile_value) const
{
	return tile_value >= TILE_UP_OFFSET;
}

bool MineField::isBomb(int tile_value) const
{
	return tile_value == BOMB || tile_value == BOMB + TILE_UP_OFFSET;
}

bool MineField::isValidTile(int x, int y) const
{
	return x >= 0 && x < width && y >= 0 && y < height;
}

int MineField::getWidth() const
{
	return width;
}

int MineField::getHeight() const
{
	return height;
}

const std::vector<int> &MineField::getGrid() const
{
	return grid;
}

int Minesweeper::MineField::getAt(int x, int y) const
{
	return grid[x + y * width];
}

void Minesweeper::MineField::setAt(int x, int y, int value)
{
	grid[x + y * width] = value;
}

int Minesweeper::MineField::getSeed() const
{
	return seed;
}
