#include <bits/stdc++.h>
using namespace std;

void print(vector<int> a) {
  printf("%d", a[0]);
  for (int i = 1; i < a.size(); i++) {
    printf(" %d", a[i]);
  }
  puts("");
}

string insert_hamming_bits(string bits, int n) {
  string ans;
  int power = 1;

  for (int i = 0, j = 0; i < n && j < bits.length(); i++) {
    if (i + 1 == power) {
      ans += '0';
      power *= 2;
    } else {
      ans += bits[j];
      j++;
    }
  }
  return ans;
}

/*
  k = number to convert,
  sz = preferred size of bitstream after conversion
*/
string decimal_to_binary_reversed(int k, int sz) {
  int req_sz = log2(k) + 1;
  if (req_sz > sz) return "size too small";
  string bin = "";
  while (k) {
    bin += (k % 2 + '0');
    k /= 2;
  }
  // add padding zeros
  for (int i = 0; i < sz - req_sz; i++) bin += '0';
  return bin;
}

int generate_parity(string data) {
  int parity = 0;
  for (int i = 0; i < data.length(); i++)
    if (data[i] == '1') parity ^= (i + 1);
  return parity;
}

string insert_parity(int parity, string data, int r) {
  string parity_binary = decimal_to_binary_reversed(parity, r);
  int power = 1;
  for (int i = 0; i < parity_binary.length(); i++) {
    data[power - 1] = parity_binary[i];
    power *= 2;
  }
  return data;
}

string hamming_code(int n, int k, string bits) {
  //  divide the bits into multiple of k
  vector<string> div;
  string ans = "";
  int r = n - k;
  for (int i = 0; i < bits.length(); i += k) {
    div.push_back(bits.substr(i, k));
  }
  for (int i = 0; i < div.size(); i++) {
    string chunk = div[i];
    chunk = insert_hamming_bits(chunk, n);
    int parity = generate_parity(chunk);
    chunk = insert_parity(parity, chunk, r);
    cout << chunk << endl;
    ans += chunk;
  }
  return ans;
}

string remove_hamming_bits(string data) {
  int power = 1;
  string res = "";
  for (int i = 0; i < data.length(); i++) {
    if (i + 1 == power) {
      power *= 2;
      continue;
    }
    res += data[i];
  }
  return res;
}

string hamming_decode(int n, int k, string encoded) {
  vector<string> div;
  string ans = "";
  int r = n - k;
  for (int i = 0; i < encoded.length(); i += n) {
    div.push_back(encoded.substr(i, n));
  }
  for (int i = 0; i < div.size(); i++) {
    string chunk = div[i];
    int parity = generate_parity(chunk);
    if (parity != 0) {
      cout << "error found in "
           << (i + 1)
           << "-th chunk , position: "
           << parity << endl;
      chunk[parity - 1] = chunk[parity - 1] == '1' ? '0' : '1';
      div[i] = chunk;
    }
  }
  for (int i = 0; i < div.size(); i++) {
    string chunk = div[i];
    chunk = remove_hamming_bits(chunk);
    ans += chunk;
  }
  return ans;
}

string inject_single_bit_error(string data, unsigned int pos) {
  data[pos] = data[pos] == '1' ? '0' : '1';
  return data;
}

int main() {
  int n, k;
  cin >> n >> k;
  string bits;
  cin >> bits;
  string encoded = hamming_code(n, k, bits);
  encoded = inject_single_bit_error(encoded, 11);
  string decoded = hamming_decode(n, k, encoded);
  cout << decoded << endl;
}