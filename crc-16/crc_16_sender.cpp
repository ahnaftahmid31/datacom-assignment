#include <arpa/inet.h>
#include <bits/stdc++.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8080

using namespace std;

// struct for a client side socket connection
struct ClientConnection {
  int sock = 0, valread;
  struct sockaddr_in serv_addr;

  char buffer[1024] = {0};
  int status = -1;
  ClientConnection(int port = PORT) {
    status = setup_connection(port);
  }

  int setup_connection(int port) {
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      printf("\n Socket creation error \n");
      return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
      printf("\nInvalid address/ Address not supported \n");
      return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
      printf("\nConnection Failed \n");
      return -1;
    }
    return 1;
  }

  void sendMessage(string msg) {
    send(sock, msg.c_str(), msg.length(), 0);
  }

  string receiveMessage() {
    string str = "";
    char buffer[65536] = {0};
    valread = read(sock, buffer, 65536);
    str = buffer;
    return str;
  }
};

// manchester encode scheme
string manchest_encode(string bits) {
  string volts = "";
  string one = "-5v, +5v, ";
  string zero = "+5v, -5v, ";

  for (int i = 0; i < bits.length(); i++) {
    volts += (bits[i] == '0') ? zero : one;
  }
  volts.resize(volts.length() - 2);
  return volts;
}

// x + 1
const vector<int> single_bit_poly{0, 1};
// x^7 + x^6 + 1
const vector<int> two_isolated_bit_poly{0, 1, 6};
// x^4 + x^2 + x + 1
const vector<int> odd_error_poly{0, 2, 1, 1};
// x^16 + x^12 + x^5 + 1 = (x + 1)(x^15 + x + 1)
const vector<int> burst_error_poly{0, 4, 7, 5};

// returns remainder after dividing with g(x)
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

// an abstract function for generating errors
string error_gen(string bits, int idx, int len) {
  idx = bits.length() - 1 - idx;
  for (int i = idx, j = len; j > 0; i--, j--) {
    bits[i] = bits[i] == '1' ? '0' : '1';
  }
  return bits;
}

// inject a custom postioned error
string custom_error_gen(string bits, vector<int> indices) {
  for (int i = bits.length() - 1, j = 0; j < indices.size(); j++) bits[i - indices[j]] = bits[i - indices[j]] == '1' ? '0' : '1';
  return bits;
}

// inject a single bit error
string single_bit_error(string bits, int i) {
  return error_gen(bits, i, 1);
}

// inject 2 isolated bits error
string two_isolated_bits_error(string bits, int i, int j) {
  int idx = bits.length() - 1 - i;
  int idx2 = bits.length() - 1 - j;
  bits[idx] = bits[idx] == '0' ? '1' : '0';
  bits[idx2] = bits[idx2] == '0' ? '1' : '0';
  return bits;
}

// inject odd length error
string odd_length_error(string bits, int start_pos, int len = 3) {
  if (len % 2 == 0) printf("%d is not odd!\n", len);
  return error_gen(bits, start_pos, len);
}

// inject burst error of certain length
string burst_error(string bits, int start_pos, int len) {
  return error_gen(bits, start_pos, len);
}

// convert a string to ascii code bits
string convert_string_to_binary(string str) {
  string bits = "";
  for (int i = 0; i < str.length(); i++) {
    bits += bitset<8>(str[i]).to_string();
  }
  return bits;
}

// returns which polynomial to use based on poly_type
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
  return single_bit_poly;
}

// returns error injected bitstream
string inject_error(int k, string bits) {
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
  return "";
}

// scan bitstream input
string take_bits_input() {
  string msg = "Hello!";
  string bits = "10101101";

  printf("\nDo you want ascii characters or custom bits?\n");
  printf("1. ASCII 2. Custom bits 3. Exit Program\n");
  int type_bit = 0;
  cin >> type_bit;
  if (type_bit == 1) {
    printf("Enter a string:\n");
    cin.ignore();
    getline(cin, msg);
    bits = convert_string_to_binary(msg);
  } else if (type_bit == 2) {
    printf("Enter bitstream:\n");
    cin >> bits;
  } else {
    return "end";
  }
  return bits;
}

// scan input for polynomial type
int take_poly_input() {
  int type_poly = 0;
  printf("\nWhat type of polynomial should we use?\n");
  printf("1. Single bit\n2. Two isolated bits\n3. Odd number of error\n4. Burst error\n");
  cin >> type_poly;
  return type_poly;
}

// scan input for error
int take_error_input() {
  printf("\nWhat type of error we inject?\n");
  printf("1. Single bit\n2. 2 Isolated bits\n3. Odd length\n4. Burst\n5. Custom\n");
  int type_err = 0;
  cin >> type_err;
  return type_err;
}

int main() {
  ClientConnection cc = ClientConnection(3001);

  // if can't connect to server
  if (cc.status < 0) return 0;

  while (true) {
    string bits = take_bits_input(); // take input
    if (bits == "end") {
      cc.sendMessage("9");
      break;
    }
    // which polynomial should use
    int type_poly = take_poly_input();
    vector<int> poly = select_poly(type_poly);

    // calculate remainder with g(x)
    string rem = getcrc16(bits, poly);
    string bits_with_rem = bits + rem;

    // inject a error
    int type_err = take_error_input();
    string errbits = inject_error(type_err, bits_with_rem);

    cout << "Original Bits:     " << bits << endl;
    cout << "Remainder:         " << rem << endl;
    cout << "Bits to be sent:   " << bits_with_rem << endl;
    cout << "After Error, Bits: " << errbits << endl;

    // encode in manchester
    string manchester = manchest_encode(errbits);
    // send encoded bits and poly type to server
    cc.sendMessage(manchester + to_string(type_poly));

  }
}