#include <iostream>
#include <string>
using namespace std;

int main()
{
    string str;
    cout << "Enter the string: ";
    cin >> str;

    int n = str.length();

    string bestPattern = "";
    string bestSuffix = "";
    int maxRepeat = 0;

    for (int len = 1; len <= n / 2; len++)
    {
        string pattern = str.substr(0, len);

        int i = 0;
        int repeat = 0;

        while (i + len <= n && str.substr(i, len) == pattern)
        {
            repeat++;
            i += len;
        }

        if (repeat >= 2)
        {
            if (repeat > maxRepeat ||
                (repeat == maxRepeat &&
                 pattern.length() > bestPattern.length()))
            {
                maxRepeat = repeat;
                bestPattern = pattern;
                bestSuffix = str.substr(i);
            }
        }
    }

    if (maxRepeat > 0)
    {
        cout << "\nSuitable CFG:\n";
        cout << "S -> " << bestPattern << "S\n";

        if (bestSuffix.empty())
            cout << "S -> ε\n";
        else
            cout << "S -> " << bestSuffix << endl;

        cout << "\nLanguage Pattern:\n";

        if (bestSuffix.empty())
            cout << "L = { (" << bestPattern << ")^n | n > 0 }\n";
        else
            cout << "L = { (" << bestPattern << ")^n "
                 << bestSuffix << " | n > 0 }\n";
    }
    else
    {
        cout << "\nNo repeating pattern found.\n";
        cout << "Suitable CFG:\n";
        cout << "S -> " << str << endl;

        cout << "\nLanguage Pattern:\n";
        cout << "L = { " << str << " }\n";
    }

    return 0;
}