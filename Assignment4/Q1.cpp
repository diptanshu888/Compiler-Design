#include <iostream>
#include <stack>
#include <string>
#include <map>
using namespace std;

int main()
{
    string input;

    cout << "Enter the input string: ";
    cin >> input;

    input += "$";

    // LL(1) Parsing Table
    map<char, map<char, string>> table;

    table['S']['a'] = "aABe";

    table['A']['b'] = "bAc";
    table['A']['d'] = "d";

    table['B']['f'] = "f";
    table['B']['e'] = "";   // epsilon

    // Parsing stack
    stack<char> st;

    st.push('$');
    st.push('S');

    int index = 0;

    cout << "\nStack\t\tInput\t\tAction\n";

    while (!st.empty())
    {
        char top = st.top();
        char current = input[index];

        // Print current stack
        stack<char> temp = st;
        string stackContent = "";

        while (!temp.empty())
        {
            stackContent += temp.top();
            temp.pop();
        }

        cout << stackContent << "\t\t"
             << input.substr(index) << "\t\t";

        // Both stack and input reach $
        if (top == '$' && current == '$')
        {
            cout << "Accepted\n";
            break;
        }

        // If top of stack is a terminal
        if (top == 'a' || top == 'b' || top == 'c' ||
            top == 'd' || top == 'e' || top == 'f' ||
            top == '$')
        {
            if (top == current)
            {
                cout << "Match " << current << endl;
                st.pop();
                index++;
            }
            else
            {
                cout << "Error\n";
                break;
            }
        }

        // If top is a non-terminal
        else
        {
            if (table[top].count(current))
            {
                string production = table[top][current];

                cout << top << " -> ";

                if (production == "")
                    cout << "epsilon\n";
                else
                    cout << production << endl;

                st.pop();

                // Push RHS in reverse order
                for (int i = production.length() - 1; i >= 0; i--)
                {
                    st.push(production[i]);
                }
            }
            else
            {
                cout << "Error: No rule found\n";
                break;
            }
        }
    }

    return 0;
}
