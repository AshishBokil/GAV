// reading a text file
#include <iostream>
#include <fstream>
#include <string>
#include <bits/stdc++.h>
using namespace std;

int main () {
  string line;
  string intermediate;
  vector<string> tokens;
  ifstream myfile ("m399.off");
  if (myfile.is_open())
  {
      getline (myfile,line);
     getline (myfile,line);
     stringstream check1(line);
      while(getline(check1, intermediate, ' '))
    {
        tokens.push_back(intermediate);
    }
    cout<<tokens[0]<<"\n";
  cout<<tokens[1]<<"\n";
cout<<tokens[2]<<"\n";

//  cout<<line<<" size="<<line.size();
    // while ( getline (myfile,line) )
    // {
    //  // cout << line << '\n';
    // }
    myfile.close();
  }

  else cout << "Unable to open file"; 

  return 0;
}