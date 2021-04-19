#include <bits/stdc++.h>
using namespace std;

// 2B1Q encode
string twobee_encode(string bits) {
  string volts = "", err = "";
  int len = bits.length();

  // error checking
  if (len % 2 != 0) {
    err = "\neven amount of bits needed! But you gave odd";
    bits.resize(len - 1);
  }

  // encoding
  for (int i = 0; i < len - 1; i += 2) {
    char a = bits[i];
    char b = bits[i + 1];
    if (a == '0' && b == '0') {
      volts += "-3, ";
    } else if (a == '0' && b == '1') {
      volts += "-1, ";
    } else if (a == '1' && b == '0') {
      volts += "+3, ";
    } else if (a == '1' && b == '1') {
      volts += "+1, ";
    } else {
      err = "\nwrong input for encode!";
      break;
    }
  }
  // removing last comma and space
  volts.resize(volts.length() - 2);
  volts += err;
  return volts;
}

// 2B1Q decode
string twobee_decode(string volts) {
  string bits;
  int len = volts.length();
  // error checking
  for (int i = 0; i < len; i += 4) {
    if (volts[i] != '+' && volts[i] != '-') {
      return "wrong input!";
    }
  }

  for (int i = 1; i < len; i += 4) {
    if (volts[i] != '1' && volts[i] != '3') {
      return "volts need to be between [-1, +3]v";
    }
  }

  // decoding
  for (int i = 0; i < len; i += 4) {
    char a = volts[i];
    char b = volts[i + 1];
    if (a == '-' && b == '3') {
      bits += "00";
    } else if (a == '-' && b == '1') {
      bits += "01";
    } else if (a == '+' && b == '3') {
      bits += "10";
    } else if (a == '+' && b == '1') {
      bits += "11";
    } else {
      return "wrong input!";
    }
  }
  return bits;
}

// MLT-3 encode
string multi_encode(string bits) {
  string volts = "";
  string cur = "+0, ";
  char last = '-';

  // encoding
  for (int i = 0; i < bits.length(); i++) {
    if (bits[i] == '0') {
      volts += cur;
    } else if (bits[i] == '1') {
      if (cur[1] != '0') {
        volts += "+0, ";
        cur = "+0, ";
      } else {
        last = (last == '-') ? '+' : '-';
        volts += last;
        volts += "5, ";
        cur = last;
        cur += "5, ";
      }
    } else {
      return "wrong input!";
    }
  }
  // removing last comma and space
  volts.resize(volts.length() - 2);
  return volts;
}

// MLT-3 decoding
string multi_decode(string volts) {
  string bits = "";
  string cur = "+0";
  int len = volts.length();
  // error checking
  for (int i = 0; i < len; i += 4) {
    if (volts[i] != '+' && volts[i] != '-') {
      return "wrong input!";
    }
  }

  for (int i = 1; i < len; i += 4) {
    if (volts[i] != '0' && volts[i] != '5') {
      return "volts need to be between (-5, 0 and +5)v";
    }
  }

  // decoding
  for (int i = 0; i < volts.length(); i += 4) {
    string nxt = volts.substr(i, 2);
    bits += (cur == nxt) ? '0' : '1';
    cur = nxt;
  }
  return bits;
}

// B8ZS encoding
string eightsub_encode(string bits) {
  string volts = "";
  string last = "-5";
  string v = last;
  string b = (v == "-5") ? "+5" : "-5";

  // encoding
  int cnt = 0;
  for (int i = 0; i < bits.length(); i++) {
    if (bits[i] == '0') {
      cnt++;
      volts += "+0, ";
      if (cnt == 8) {
        cnt = 0;
        int pos = volts.length() - 20;
        volts.replace(pos, 2, v);
        pos += 4;
        volts.replace(pos, 2, b);
        pos += 8;
        volts.replace(pos, 2, b);
        pos += 4;
        volts.replace(pos, 2, v);
      }
    } else if (bits[i] == '1') {
      cnt = 0;
      b = last;
      last = (last == "-5") ? "+5" : "-5";
      v = last;
      volts += last + ", ";
    } else {
      return "wrong input!";
    }
  }
  // removing last comma and space
  volts.resize(volts.length() - 2);
  return volts;
}

// B8ZS decode
string eightsub_decode(string volts) {
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
  // if more than eight zero found then error
  int cnt = 0;
  for (int i = 0; i < volts.length(); i += 4) {
    string cur = volts.substr(i, 2);
    if (cur == "+0")
      cnt++;
    else
      cnt = 0;
    if (cnt >= 8) return "wrong scheme! consecutive 8 or more zeros!";
  }

  // pattern checking
  cnt = 0;
  pre = "-5";
  int len = volts.length();
  for (int i = 0; i < volts.length(); i += 4) {
    string cur = volts.substr(i, 2);
    if (cur == "+0") {
      cnt++;
    } else {
      if (cur == pre) {
        string g = cur;
        string h = cur == "+5" ? "-5" : "+5";
        // should be 3 consecutive zero before
        if (cnt != 3) return "Wrong scheme!";
        // next 4 volts must have this pattern : h0hg (B0VB)
        i += 4;
        if (i >= len) continue;
        string volt = volts.substr(i, 2);
        if (volt != h) return "Wrong Scheme!";
        i += 4;
        if (i >= len) continue;
        volt = volts.substr(i, 2);
        if (volt != "+0") return "Wrong Scheme!";
        i += 4;
        if (i >= len) continue;
        volt = volts.substr(i, 2);
        if (volt != h) return "Wrong Scheme!";
        i += 4;
        if (i >= len) continue;
        volt = volts.substr(i, 2);
        if (volt != g) return "Wrong Scheme!";
      }

      pre = cur;
      cnt = 0;
    }
  }

  // decoding
  int i = 0;
  while (i < volts.length()) {
    string nxt = volts.substr(i, 2);
    if (nxt == "+0") {
      bits += '0';
      i += 4;
    } else if (nxt == "+5" || nxt == "-5") {
      if (nxt == last) {
        for (int k = 0; k < 5; k++) {
          bits += '0';
        }
        i += 20;
      } else {
        bits += '1';
        last = (last == "-5") ? "+5" : "-5";
        i += 4;
      }
    } else {
      return "wrong input!";
    }
  }
  return bits;
}

// HDB3 encoding
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

// show the text for what to do
void input_text() {
  puts("Enter a number between 1-9:");
  puts("1. 2B1Q encode,    2. 2B1Q decode,");
  puts("3. MLT-3 encode,   4. MLT-3 decode,");
  puts("5. B8ZS encode,    6. B8ZS decode,");
  puts("7. HDB3 encode,    8. HDB3 decode");
  puts("9. Exit");
}

// takes input from the user
int take_input() {
  int n;
  cin >> n;
  if (n == 9) return -1;
  cin.ignore();
  string inp;
  getline(cin, inp);

  switch (n) {
    case 1:
      cout << twobee_encode(inp) << endl;
      break;
    case 2:
      cout << twobee_decode(inp) << endl;
      break;
    case 3:
      cout << multi_encode(inp) << endl;
      break;
    case 4:
      cout << multi_decode(inp) << endl;
      break;
    case 5:
      cout << eightsub_encode(inp) << endl;
      break;
    case 6:
      cout << eightsub_decode(inp) << endl;
      break;
    case 7:
      cout << highdense_encode(inp) << endl;
      break;
    case 8:
      cout << highdense_decode(inp) << endl;
      break;
    default:
      break;
  }
  return 1;
}

int main() {
  while (true) {
    input_text();
    int x = take_input();
    if (x == -1) break;
    puts("");
  }
}