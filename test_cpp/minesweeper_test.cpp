#include "minesweeper.h"
#include <algorithm>
#include <gtest/gtest.h>

namespace Minesweeper {
    // Test Fixture for Minesweeper::MineField
    class MineFieldTest : public ::testing::Test {
    protected:
        void SetUp() override {

        }

        void TearDown() override {

        }
    };

    // Test the MineField constructor
    TEST_F(MineFieldTest, ConstructorTest) {
        MineField minefield(5, 5);
        EXPECT_EQ(minefield.getWidth(), 5);
        EXPECT_EQ(minefield.getHeight(), 5);
        EXPECT_EQ(minefield.getSeed(), 0);  // Assuming default seed is 0
        EXPECT_EQ(minefield.getGrid().size(), 25);  // 5x5 grid
    }

    // Test the toString function
    TEST_F(MineFieldTest, ToStringTest) {
        MineField minefield(5, 5);
        std::string result = minefield.toString();
        EXPECT_FALSE(result.empty());  // Ensure the string is not empty
        // Optionally, test the specific output format here
    }

    // Test the flag function (right-clicking a tile)
    TEST_F(MineFieldTest, FlagTest) {
        MineField minefield(1, 1);
        int result = minefield.flag(0, 0);  // Flag a tile at (2, 2)
        EXPECT_EQ(result, FLAG_OFFSET);  // Assuming the function returns a flag offset value
        result = minefield.flag(0, 0);  // Flag a tile at (2, 2)
        EXPECT_EQ(result, TILE_UP_OFFSET);  
    }

    // Test the clear function (clear all tiles)
    TEST_F(MineFieldTest, ClearTest) {
        MineField minefield(5, 5);
        minefield.sweep(0, 0);
        FAIL();
        for(int x = 0; x < minefield.getWidth(); ++x) {
            for(int y = 0; x < minefield.getHeight(); ++y) {
                auto sweptTiles = minefield.sweep(x, y);
                bool hitBomb = sweptTiles[0].value == BOMB;
                std::cout << minefield.toString() << '\n';
                if(hitBomb) {
                    break;
                }
            }
        }
        //minefield.clear();
        auto isFreshTile = [](int tile_value){ return tile_value == TILE_UP_OFFSET; };
        auto raw_grid = minefield.getGrid();
        bool isFreshGrid = std::all_of(raw_grid.begin(), raw_grid.end(), isFreshTile);
        EXPECT_TRUE(isFreshGrid);
        FAIL();
    }

    // Test for first sweep (typically, the first sweep should not be a bomb)
    TEST_F(MineFieldTest, FirstSweepTest) {
        MineField minefield(5, 5);
        std::vector<Tile> swept = minefield.sweep(0, 0);  // Sweep the first tile
        auto isTileBomb = [](const Tile& tile){ return tile.value == BOMB; };
        bool hasNoBombs = std::find_if(swept.begin(), swept.end(), isTileBomb) == swept.end();
        EXPECT_TRUE(hasNoBombs); 
    }

    // Test invalid tile position
    TEST_F(MineFieldTest, InvalidTileTest) {
        MineField minefield(5, 5);
        try {
            minefield.sweep(6, 6);
            FAIL() << "Expected std::out_of_range";
        }
        catch(std::out_of_range const & err) {
            SUCCEED();
        }
        catch(...) {
            FAIL() << "Expected std::out_of_range";
        }
    }
}
