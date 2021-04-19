#include <bits/stdc++.h>
using namespace std;

string ami_encode() {
  string volts = "";
  string bits;
  cin >> bits;
  int one = -1;
  for (int i = 0; i < bits.length(); i++) {
    if (bits[i] == '0') {
      volts += "+0v, ";
      continue;
    }
    volts += (one > 0) ? "-5v, " : "+5v, ";
    one *= -1;
  }
  volts.resize(volts.length() - 2);
  return volts;
}

string ami_decode() {
  string volts;
  getline(cin, volts);
  string bits = "";
  char prev = '-';
  for (int i = 0; i < volts.length(); i++) {
    if (volts[i] == '5') {
      if (volts[i-1] == prev) {
        return "wrong input scheme";
      } else {
        prev = (prev == '-') ? '+':'-';
      }
    }
  }
  for (int i = 1; i < volts.length(); i += 5) {
    bits += (volts[i] == '0') ? "0" : "1";
  }
  return bits;
}

string manchest_encode() {
  string volts = "";
  string bits;
  cin >> bits;
  string one = "-5v, +5v, ";
  string zero = "+5v, -5v, ";

  for (int i = 0; i < bits.length(); i++) {
    volts += (bits[i] == '0') ? zero : one;
  }
  volts.resize(volts.length() - 2);
  return volts;
}

string manchest_decode() {
  string bits = "";
  string volts;
  getline(cin, volts);

  for (int i = 0; i < volts.length(); i+= 10) {
    if (volts[i] == volts[i+3]) return "wrong scheme!";
  }

  for (int i = 0; i < volts.length(); i += 10) {
    bits += (volts[i] == '+') ? "0" : "1";
  }
  return bits;
}

string diffman_encode() {
  string volts = "";
  string bits;
  cin >> bits;
  string a = "-5v, +5v, ";
  string b = "+5v, -5v, ";
  string prev = a;
  for (int i = 0; i < bits.length(); i++) {
    if (bits[i] == '1') {
      prev = (prev == a) ? b : a;
    }
    volts += prev;
  }
  volts.resize(volts.length()-2);
  return volts;
}

string diffman_decode() {
  string bits = "";
  string volts;
  getline(cin, volts);
  char prev = '+';

  for (int i = 0; i < volts.length(); i += 10) {
    if (volts[i] == volts[i + 3])
      return "wrong scheme!";
  }

  for (int i = 0; i < volts.length(); i += 10) {
    if (volts[i] == prev) {
      bits += "1";
      prev = (prev == '+') ? '-' : '+';
    } else {
      bits += "0";
    }
  }
  return bits;
}

void input_msg() {
  printf("Please enter a number between 1 to 6\nbased on what you want to operate:\n");
  printf("1. AMI Encode\n");
  printf("2. AMI Decode\n");
  printf("3. Manchester Encode\n");
  printf("4. Manchester Decode\n");
  printf("5. Differential Manchester Encode\n");
  printf("6. Differential Manchester Decode\n");
  printf("7. Exit\n");
  puts("");
}

string take_input() {
  int n;
  string ans;
  cin >> n;
  cin.ignore();
  switch (n) {
  case 1:
    ans = ami_encode();
    break;
  case 2:
    ans = ami_decode();
    break;
  case 3:
    ans = manchest_encode();
    break;
  case 4:
    ans = manchest_decode();
    break;
  case 5:
    ans = diffman_encode();
    break;
  case 6:
    ans = diffman_decode();
    break;
  case 7:
    puts("Happy Coding! :)");
    return "7";
  default:
    ans = "input wrong!";
    break;
  }
  return ans;
}

int main() {
  string ans;
  while (1) {
    //input_msg();
    ans = take_input();
    if (ans[0] == '7') break;
    cout << ans << endl;
    puts("");
  }
}