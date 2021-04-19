#include<bits/stdc++.h>
using namespace std;

int main() {
  string input, output = "";
  cin >> input;
  for (int i = 0; i<input.length(); i = i+3) {
    if (input[i] == '+') {
      output += '1';
    } else { 
      output += '0';
    }
  }
  cout << output << endl;
}