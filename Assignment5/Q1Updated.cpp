#include <bits/stdc++.h>
using namespace std;

struct I{
    int p,d;
    bool operator<(const I& x)const{
        return p!=x.p?p<x.p:d<x.d;
    }
};

vector<pair<char,string>> g={{'Z',"S"},{'S',"A"},{'A',"Ab"},{'A',"a"}};

bool nt(char c){
    return c=='Z'||c=='S'||c=='A';
}

set<I> closure(set<I> s){
    bool f=true;
    while(f){
        f=false;
        for(auto x:s){
            int p=x.p,d=x.d;
            if(d<g[p].second.size()&&nt(g[p].second[d])){
                char c=g[p].second[d];
                for(int i=0;i<g.size();i++){
                    if(g[i].first==c){
                        I y={i,0};
                        if(!s.count(y)){
                            s.insert(y);
                            f=true;
                        }
                    }
                }
            }
        }
    }
    return s;
}

set<I> go(set<I> s,char c){
    set<I> t;
    for(auto x:s){
        int p=x.p,d=x.d;
        if(d<g[p].second.size()&&g[p].second[d]==c)
            t.insert({p,d+1});
    }
    return closure(t);
}

string key(set<I> s){
    string r;
    for(auto x:s)
        r+=to_string(x.p)+","+to_string(x.d)+";";
    return r;
}

string item(I x){
    int p=x.p,d=x.d;
    string r;
    r+=g[p].first;
    r+=" -> ";
    for(int i=0;i<=g[p].second.size();i++){
        if(i==d)r+=".";
        if(i<g[p].second.size())r+=g[p].second[i];
    }
    return r;
}

void show(set<I> s,int id){
    cout<<"I"<<id<<":\n";
    for(auto x:s)
        cout<<"    "<<item(x)<<"\n";
}

string getAction(map<char,string>& a,char c){
    if(a.count(c))return a[c];
    return "-";
}

string getGoto(map<char,int>& h,char c){
    if(h.count(c))return to_string(h[c]);
    return "-";
}

int main(){
    set<I> s=closure({{0,0}});
    vector<set<I>> v;
    map<string,int> id;
    v.push_back(s);
    id[key(s)]=0;

    for(int i=0;i<v.size();i++){
        set<char> c;
        for(auto x:v[i]){
            int p=x.p,d=x.d;
            if(d<g[p].second.size())
                c.insert(g[p].second[d]);
        }
        for(char x:c){
            set<I> t=go(v[i],x);
            if(t.empty())continue;
            string k=key(t);
            if(!id.count(k)){
                int z=v.size();
                id[k]=z;
                v.push_back(t);
            }
        }
    }

    cout<<"\n==============================================\n";
    cout<<"       CANONICAL COLLECTION OF LR(0) ITEMS\n";
    cout<<"==============================================\n";

    for(int i=0;i<v.size();i++){
        show(v[i],i);
        cout<<"\n";
    }

    vector<map<char,string>> a(v.size());
    vector<map<char,int>> h(v.size());

    for(int i=0;i<v.size();i++){
        set<char> c;

        for(auto x:v[i]){
            int p=x.p,d=x.d;
            if(d<g[p].second.size())
                c.insert(g[p].second[d]);
        }

        for(char x:c){
            set<I> t=go(v[i],x);
            if(t.empty())continue;

            int j=id[key(t)];

            if(nt(x))
                h[i][x]=j;
            else
                a[i][x]="s"+to_string(j);
        }

        for(auto x:v[i]){
            int p=x.p,d=x.d;

            if(d==g[p].second.size()){
                if(p==0){
                    a[i]['$']="acc";
                }
                else{
                    string r="r"+to_string(p);

                    for(char x:{'a','b','$'}){
                        if(a[i].count(x)&&x=='b')
                            continue;
                        a[i][x]=r;
                    }
                }
            }
        }
    }

    cout<<"==============================================\n";
    cout<<"             LR(0) PARSING TABLE\n";
    cout<<"==============================================\n\n";

    cout<<"+-------+----------+----------+----------+----------+----------+\n";
    cout<<"| State |    a     |    b     |    $     |    S     |    A     |\n";
    cout<<"+-------+----------+----------+----------+----------+----------+\n";

    for(int i=0;i<v.size();i++){
        cout<<"| "<<setw(5)<<i<<" | "
            <<setw(8)<<getAction(a[i],'a')<<" | "
            <<setw(8)<<getAction(a[i],'b')<<" | "
            <<setw(8)<<getAction(a[i],'$')<<" | "
            <<setw(8)<<getGoto(h[i],'S')<<" | "
            <<setw(8)<<getGoto(h[i],'A')<<" |\n";
    }

    cout<<"+-------+----------+----------+----------+----------+----------+\n";

    cout<<"\nProductions:\n";
    cout<<"r1 = S -> A\n";
    cout<<"r2 = A -> Ab\n";
    cout<<"r3 = A -> a\n";

    cout<<"\nNote: State 2 has a shift/reduce conflict on 'b'.\n";
    cout<<"For parsing 'abb', SHIFT is chosen on 'b'.\n";

    string w="abb$";
    vector<string> st={"0"};
    int i=0;

    cout<<"\n==============================================\n";
    cout<<"              PARSING INPUT: abb\n";
    cout<<"==============================================\n\n";

    cout<<"+----------------------+----------+----------------+\n";
    cout<<"| Stack                | Input    | Action         |\n";
    cout<<"+----------------------+----------+----------------+\n";

    while(true){
        int q=stoi(st.back());
        char c=w[i];
        string x=a[q][c];

        string stack;

        for(auto z:st){
            if(!stack.empty())stack+=" ";
            stack+=z;
        }

        string action=x;

        if(x=="acc"){
            cout<<"| "<<left<<setw(20)<<stack<<" | "
                <<setw(8)<<w.substr(i)<<" | "
                <<setw(14)<<"ACCEPT"<<" |\n";
            break;
        }

        if(x.empty()){
            cout<<"| "<<left<<setw(20)<<stack<<" | "
                <<setw(8)<<w.substr(i)<<" | "
                <<setw(14)<<"ERROR"<<" |\n";
            break;
        }

        if(x[0]=='s'){
            cout<<"| "<<left<<setw(20)<<stack<<" | "
                <<setw(8)<<w.substr(i)<<" | "
                <<setw(14)<<"Shift "+string(1,c)<<" | \n";

            int j=stoi(x.substr(1));
            st.push_back(string(1,c));
            st.push_back(to_string(j));
            i++;
        }
        else if(x[0]=='r'){
            int p=stoi(x.substr(1));
            char l=g[p].first;
            string r=g[p].second;

            string act="Reduce "+string(1,l)+"->"+r;

            cout<<"| "<<left<<setw(20)<<stack<<" | "
                <<setw(8)<<w.substr(i)<<" | "
                <<setw(14)<<act<<" |\n";

            for(int j=0;j<2*r.size();j++)
                st.pop_back();

            q=stoi(st.back());

            st.push_back(string(1,l));
            st.push_back(to_string(h[q][l]));
        }
    }

    cout<<"+----------------------+----------+----------------+\n";
    cout<<"\nString accepted successfully.\n";

    return 0;
}
