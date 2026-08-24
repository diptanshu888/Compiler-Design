#include <iostream>
using namespace std;

int main()
{
    cout << "Original Grammar:\n\n";

    cout << "E -> E + T | T\n";
    cout << "T -> T * F | F\n";
    cout << "F -> ( E ) | id\n";

    cout << "\nAfter Removing Left Recursion:\n\n";

    cout << "E  -> T E'\n";
    cout << "E' -> + T E' | epsilon\n\n";

    cout << "T  -> F T'\n";
    cout << "T' -> * F T' | epsilon\n\n";

    cout << "F  -> ( E ) | id\n";

    return 0;
}
