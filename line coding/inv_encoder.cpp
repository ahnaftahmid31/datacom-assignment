#include<bits/stdc++.h>
using namespace std;

int main() {
  string input, output = "";
  string cur_sig, prev_sig = "+5v";  // 1 -> +5v, -1 -> -5v
  
  cin >> input;

  for (int i = 0; i<input.length(); i++) {
    int curbit = input[i] - '0';
    
    if (curbit == 1) {
      cur_sig = (prev_sig == "+5v") ? "-5v":"+5v";
    } else {
      cur_sig = prev_sig;
    }
    output += cur_sig;
    prev_sig = cur_sig;
  }
  cout << output << endl;
}