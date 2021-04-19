#include <bits/stdc++.h>

using namespace std;

int aschii_length = 8;

// HDB3 decode
string highdense_decode(string volts) {
  string bits = "";
  string last = "-5";

  // error checking
  // immediate previous non-zero voltage (i-1)
  // adjacent non-xero voltage can't be same
  string pre = last;
  for (int i = 0; i < volts.length(); i += 4) {
    string cur = volts.substr(i, 2);
    if (pre == cur && cur[1] == '5') {
      return "wrong scheme!";
    }
    pre = cur;
  }
  // consecutive 4 zero error
  int cnt = 0;
  for (int i = 0; i < volts.length(); i += 4) {
    string cur = volts.substr(i, 2);
    if (cur == "+0")
      cnt++;
    else
      cnt = 0;
    if (cnt >= 4) return "wrong scheme! consecutive 4 or more zeros!";
  }
  // pattern checking
  cnt = 0;
  int non_cnt = 0;
  pre = "-5";
  for (int i = 0; i < volts.length(); i += 4) {
    string cur = volts.substr(i, 2);
    if (cur == "+0")
      cnt++;
    else {
      non_cnt++;
      if (cur == pre) {
        // violation 000V
        if (cnt == 3) {
          if (non_cnt % 2 != 0) return "Wrong Scheme! Odd non-zero after subs";
        }
        // violation B00V
        else if (cnt == 2) {
          if (non_cnt % 2 != 0) return "Wrong Scheme! Odd non-zero after subs";
        } else {
          return "Wrong scheme, can't decode!";
        }
      }
      cnt = 0;
      pre = cur;
    }
  }

  // decoding
  for (int i = 0; i < volts.length(); i += 4) {
    string a = volts.substr(i, 2);
    if (a == "+0") {
      bits += '0';
    } else if (a == "+5" || a == "-5") {
      if (last != a) {
        last = (last == "-5") ? "+5" : "-5";
        bits += '1';
      } else {
        int pos = bits.length() - 3;
        bits.replace(pos, 3, "000");
        bits += '0';
      }
    } else {
      return "wrong input!";
    }
  }
  return bits;
}

string two_dimensional_parity_decode(string bits) {
  if (bits[0] != '0' && bits[0] != '1') return "";
  // make it 2D with 9 bits on each row
  vector<string> rows;
  int row_cnt = 0;
  string str;
  for (int i = 0; i < bits.length(); i++) {
    str += bits[i];
    if ((i + 1) % (aschii_length + 1) == 0) {
      row_cnt++;
      rows.push_back(str);
      // cout << str << endl;
      str = "";
    }
  }

  // check error for each 4 row
  int row_check[10], col_check[10], r = 0;
  string row_par, col_par;
  memset(row_check, 0, sizeof(row_check));
  memset(col_check, 0, sizeof(col_check));

  for (int i = 0, r = 0; i < rows.size(); i++, r++) {
    // we have reached the 5-th row or parity row
    if (r + 1 == 5) {
      // loading col parity
      col_par = rows[i];

      // initializing some variables
      bool row_err[10], col_err[10], has_error = false;
      memset(row_err, 0, sizeof(row_err));
      memset(col_err, 0, sizeof(col_err));
      // checking rows (ignoring last row cz that's parity row)
      for (int j = 0; j < 4; j++) {
        if (row_check[j] % 2 != (row_par[j] - '0')) {
          row_err[j] = true;
          has_error = true;
          printf("error found at row %d\n", j + 1);
        }
      }

      // checking columns (ignoring last column cz that's parity col)
      for (int j = 0; j < aschii_length; j++) {
        if (col_check[j] % 2 != (col_par[j] - '0')) {
          col_err[j] = true;
          has_error = true;
          printf("error found at col %d, col_check = %d, col_par = %c\n", j + 1,
                 col_check[j], col_par[j]);
        }
      }

      // correct if possible
      if (has_error) {
        for (int j = 0; j < 4; j++) {
          for (int k = 0; k < aschii_length; k++) {
            if (row_err[j] && col_err[k]) {
              printf("correcting error at %d row and %d column", j + 1, k + 1);
              int row_pos = i - 4 + j;
              rows[row_pos][k] = rows[row_pos][k] == '1' ? '0' : '1';
            }
          }
        }
      }

      // after all done
      r = 0;
      memset(row_check, 0, sizeof(row_check));
      memset(col_check, 0, sizeof(col_check));
    }
    // non-parity rows
    else {
      for (int j = 0; j < aschii_length + 1; j++) {
        if (rows[i][j] == '1') {
          row_check[r]++;  // 4 row will be checked
          col_check[j]++;  // 8 columns will be checked
        }
        // row parity (9th bit)
        if (j == aschii_length) row_par += rows[i][j];
      }
    }
  }

  // decode parity
  string decode_bits;
  for (int i = 0; i < rows.size(); i++) {
    if ((i + 1) % 5 == 0) continue;
    for (int j = 0; j < aschii_length; j++) {
      decode_bits += rows[i][j];
    }
  }
  return decode_bits;
}

int main() {
  string volts;
  getline(cin, volts);
  cout << two_dimensional_parity_decode(highdense_decode(volts)) << endl;
}