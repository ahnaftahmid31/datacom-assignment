#include <bits/stdc++.h>
using namespace std;

const int MAX_ROW = 65536 + 5;
int aschii_length = 8;

string highdense_encode(string bits) {
  string volts = "";
  string last = "-5";

  // encoding
  int noncnt = 0;
  int zerocnt = 0;
  for (int i = 0; i < bits.length(); i++) {
    if (bits[i] == '1') {
      noncnt++;
      zerocnt = 0;
      last = (last == "-5") ? "+5" : "-5";
      volts += last + ", ";
    } else if (bits[i] == '0') {
      zerocnt++;
      volts += "+0, ";
      if (zerocnt == 4) {
        zerocnt = 0;
        if (noncnt % 2 == 0) {
          noncnt += 2;
          int pos = volts.length() - 4 * 4;
          last = (last == "-5") ? "+5" : "-5";
          volts.replace(pos, 2, last);
          pos += 12;
          volts.replace(pos, 2, last);
        } else {
          noncnt++;
          int pos = volts.length() - 4 * 1;
          volts.replace(pos, 2, last);
        }
      }
    } else {
      return "Wrong input!";
    }
  }
  // removing last comma and space
  volts.resize(volts.length() - 2);
  return volts;
}


string row[MAX_ROW];
string two_dimensional_parity_encode(string bits) {
  for (int i = 0; i < MAX_ROW; i++) row[i].clear();
  int cnt = 0, rc = 0;
  for (int i = 0; i < bits.length(); i++) {
    if (bits[i] == '1') cnt++;
    row[rc] += bits[i];
    if ((i + 1) % aschii_length == 0) {
      if (cnt % 2 == 0) {
        row[rc] += '0';
      } else {
        row[rc] += '1';
      }
      cnt = 0;
      rc++;
    }
  }
  int col_par[10] = {0};
  memset(col_par, 0, sizeof(col_par));
  string ans;
  for (int i = 0; i <= rc; i++) {
    for (int j = 0; j < aschii_length + 1; j++) {
      if (row[i][j] == '1') {
        col_par[j]++;
      }
    }
    ans.append(row[i]);
    if ((i + 1) % 4 == 0) {
      for (int j = 0; j < aschii_length + 1; j++) {
        if (col_par[j] % 2 == 0) {
          ans += '0';
        } else {
          ans += '1';
        }
      }
      memset(col_par, 0, sizeof(col_par));
    }
  }
  for (int i = 0; i < ans.length(); i++) {
    printf("%c", ans[i]);
    if ((i + 1) % (aschii_length + 1) == 0) puts("");
  }
  return ans;
}


int main() {
  string bits, volts;
  cin >> bits;
  string parit = two_dimensional_parity_encode(bits);
  cout << highdense_encode(parit) << endl;
}