#include<bits/stdc++.h>
using namespace std;

int main() {
  string bitstream, signal = "";
  cin >> bitstream;
  for (int i = 0; i<bitstream.length(); i++) {
    int data = bitstream[i] - '0';
    if (data == 1) {
      signal += "+5v";
    } else {
      signal += "-5v";
    }
  }
  cout << signal << endl;
}