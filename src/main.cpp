#include "Minesweeper.h"
#include <iostream>

using namespace std;
using namespace Minesweeper;

int main()
{
    MineField field(10, 10);

    cout << field.toString() << '\n';
    field.sweep(5, 5);
    cout <<  field.toString() << '\n';

    return 0;
}