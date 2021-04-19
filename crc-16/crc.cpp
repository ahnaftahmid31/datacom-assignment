#include <bits/stdc++.h>
using namespace std;

// x + 1
const vector<int> single_bit_poly{0, 1};
// x^7 + x^6 + 1
const vector<int> two_isolated_bit_poly{0, 1, 6};
// x^4 + x^2 + x + 1
const vector<int> odd_error_poly{0, 2, 1, 1};
// x^16 + x^12 + x^5 + 1 = (x + 1)(x^15 + x + 1)
const vector<int> burst_error_poly{0, 4, 7, 5};

string getcrc16(string bits, vector<int> poly) {
  if (bits.length() % 8 != 0) return "bits length is not multiple of 8";

  bits += bitset<16>().to_string();
  int len = 0;
  for (int val : poly) len += val;
  for (int i = 0; i < bits.length() - len; i++) {
    if (bits[i] == '1') {
      int idx = i;
      for (int j = 0; j < poly.size(); j++) {
        idx += poly[j];
        bits[idx] = bits[idx] == '1' ? '0' : '1';
      }
    }
  }
  return bits.substr(bits.length() - 16, 16);
}

string crcdecode(string bits, vector<int> poly) {
  int len = 0;
  for (int val : poly) len += val;
  for (int i = 0; i < bits.length() - len; i++) {
    if (bits[i] == '1') {
      int idx = i;
      for (int j = 0; j < poly.size(); j++) {
        idx += poly[j];
        bits[idx] = bits[idx] == '1' ? '0' : '1';
      }
    }
  }
  return bits.substr(bits.length() - 16, 16);
}

string error_gen(string bits, int idx, int len) {
  idx = bits.length() - 1 - idx;
  for (int i = idx, j = len; j > 0; i--, j--) {
    bits[i] = bits[i] == '1' ? '0' : '1';
  }
  return bits;
}

string custom_error_gen(string bits, vector<int> indices) {
  for (int i = bits.length() - 1, j = 0; j < indices.size(); j++) bits[i - indices[j]] = bits[i - indices[j]] == '1' ? '0' : '1';
  return bits;
}

string single_bit_error(string bits, int i) {
  return error_gen(bits, i, 1);
}
string two_isolated_bits_error(string bits, int i, int j) {
  int idx = bits.length() - 1 - i;
  int idx2 = bits.length() - 1 - j;
  bits[idx] = bits[idx] == '0' ? '1' : '0';
  bits[idx2] = bits[idx2] == '0' ? '1' : '0';
  return bits;
}


string odd_length_error(string bits, int start_pos, int len = 3) {
  if (len % 2 == 0) printf("%d is not odd!\n", len);
  return error_gen(bits, start_pos, len);
}

string burst_error(string bits, int start_pos, int len) {
  return error_gen(bits, start_pos, len);
}

string convert_string_to_binary(string str) {
  string bits = "";
  for (int i = 0; i < str.length(); i++) {
    bits += bitset<8>(str[i]).to_string();
  }
  return bits;
}

vector<int> select_poly(int k) {
  switch (k) {
    case 1:
      return single_bit_poly;
    case 2:
      return two_isolated_bit_poly;
    case 3:
      return odd_error_poly;
    case 4:
      return burst_error_poly;
    default:
      break;
  }
}

string select_error(int k, string bits) {
  switch (k) {
    case 1: {
      printf("Enter error bit position:\n");
      int pos;
      cin >> pos;
      return single_bit_error(bits, pos);
    }
    case 2: {
      printf("Enter first index and second:\n");
      int idx1, idx2;
      cin >> idx1 >> idx2;
      return two_isolated_bits_error(bits, idx1, idx2);
    }
    case 3: {
      printf("Enter starting pos and length(should be odd):\n");
      int st, len;
      cin >> st >> len;
      return odd_length_error(bits, st, len);
    }
    case 4: {
      printf("Enter starting pos and length\n");
      int st, len;
      cin >> st >> len;
      return burst_error(bits, st, len);
    }
    case 5: {
      printf("Enter number of errors and all the index(0 based)\n");
      int n, idx;
      cin >> n;
      vector<int> indices;
      for (int j = 0; j < n; j++) cin >> idx, indices.push_back(idx);
      return custom_error_gen(bits, indices);
    }
    default:
      break;
  }
}

string take_bits_input() {
  string msg = "Hello!";
  string bits = "10101101";

  printf("Do you want ascii characters or custom bits?\n");
  printf("1. ASCII 2. Custom bits\n");
  int type_bit = 0;
  cin >> type_bit;
  if (type_bit == 1) {
    printf("Enter a string:\n");
    cin.ignore();
    getline(cin, msg);
    bits = convert_string_to_binary(msg);
  } else {
    printf("Enter bitstream:\n");
    cin >> bits;
  }
  return bits;
}

int take_poly_input() {
  int type_poly = 0;
  printf("What type of polynomial should we use?\n");
  printf("1. Single bit\n2. Two isolated bits\n3. Odd number of error\n4. Burst error\n");
  cin >> type_poly;
  return type_poly;
}

int take_error_input() {
  printf("What type of error we inject?\n");
  printf("1. Single bit\n2. 2 Isolated bits\n3. Odd length\n4. Burst\n5. Custom\n");
  int type_err = 0;
  cin >> type_err;
  return type_err;
}

int main() {
  string bits = take_bits_input();
  int type_poly = take_poly_input();

  vector<int> poly = select_poly(type_poly);

  string rem = getcrc16(bits, poly);
  string bits_with_rem = bits + rem;

  int type_err = take_error_input();
  string errbits = select_error(type_err, bits_with_rem);
  string errRem = crcdecode(errbits, poly);

  cout << "Original Bits:     " << bits << endl;
  cout << "Remainder:         " << rem << endl;
  cout << "Bits to be sent:   " << bits_with_rem << endl;
  cout << "After Error, Bits: " << errbits << endl;
  cout << "Error's Remainder: " << errRem << endl;
}