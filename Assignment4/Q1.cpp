#include <iostream>
#include <stack>
#include <string>
#include <map>
#include <iomanip>

using namespace std;

int main()
{
    string input;

    cout << "Enter the input string: ";
    cin >> input;

    input += "$";
    
    
        map<char, map<char, string>> table;

    table['S']['a'] = "aABe";

    table['A']['b'] = "bAc";
    table['A']['d'] = "d";

    table['B']['f'] = "f";
    table['B']['e'] = "";
    
    
    
    cout<<"LL(1) parsing table\n\n\n";
    
    cout<<setw(12)<<" "
    <<setw(12)<<"a"
    <<setw(12)<<"b"
    <<setw(12)<<"c"
    <<setw(12)<<"d"
    <<setw(12)<<"e"
    <<setw(12)<<"f"
    <<setw(12)<<"$"<<endl;
    
    cout<<string(96,'-')<<endl;
    
    char nonT[]={'S','A','B'};
    char term[]={'a','b','c','d','e','f','$'};
    
    for(char nt:nonT){
      cout<<setw(12)<<nt;
      for(char t:term){
      if(table[nt].count(t)){
      string production=table[nt][t];
      
      cout<<setw(12)<<nt+string("->")+production;
      
      }
      else{
      cout<<setw(12)<<"";
      }
      }
      cout<<endl;
      }



    
    stack<char> st;

    st.push('$');
    st.push('S');

    int index = 0;

    cout << "\nStack\t\tInput\t\tAction\n";

    while (!st.empty())
    {
        char top = st.top();
        char current = input[index];

        
        stack<char> temp = st;
        string stackContent = "";

        while (!temp.empty())
        {
            stackContent += temp.top();
            temp.pop();
        }

        cout << stackContent << "\t\t"
             << input.substr(index) << "\t\t";

        
        if (top == '$' && current == '$')
        {
            cout << "Accepted\n";
            break;
        }

        
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
