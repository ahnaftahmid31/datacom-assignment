#include <bits/stdc++.h>
using namespace std;

int main() {
  string input, output = "";
  string cur_sig, prev_sig = "+5v";  
  
  cin >> input;

  for (int i = 0; i < input.length(); i = i+3) {
    cur_sig = input.substr(i, 3);
    if (cur_sig != prev_sig) {
      output += '1';
    } else {
      output += '0';
    }
    prev_sig = cur_sig;
  }
  cout << output << endl;
}