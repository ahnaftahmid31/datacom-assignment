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
  ClientConnection(int port = PORT) { status = setup_connection(port); }

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

  /**
   *@brief sends message to server
   *@param msg message to be sent
   */
  void sendMessage(string msg) { send(sock, msg.c_str(), msg.length(), 0); }

  /**
   *@brief receives message from server
   */
  string receiveMessage() {
    string str = "";
    char buffer[65536] = {0};
    valread = read(sock, buffer, 65536);
    str = buffer;
    return str;
  }
};

/**
 *@brief converts bitstream to manchester schemed voltage level
 *@param bits bitstream
 */
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

/**
 *@brief To insert hamming parity bits we must create space for them,
 * This function inserts blank space in the power of two and pushes
 * other bits to right. Returns the resultant bitstream
 *@param bits original databits
 *@param n codeword length
 */
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

/**
 * @brief converts decimal number to binary but returns the result in reversed
 * order
 * @param k  number to convert,
 * @param sz preferred size of bitstream after conversion
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

/**
 * @brief generates the hamming parity set but in decimal form
 * @param data bitstream
 */
int generate_parity(string data) {
  int parity = 0;
  for (int i = 0; i < data.length(); i++)
    if (data[i] == '1') parity ^= (i + 1);
  return parity;
}

/**
 * @brief inserts parity bits in the position with power of 2
 * @param parity set of parity bits but in decimal form
 * @param data databits with blank in the position with power of 2
 * @param r codelength - messagelength => (n - k)
 * @return bitstream after insertion of parity
 */
string insert_parity(int parity, string data, int r) {
  string parity_binary = decimal_to_binary_reversed(parity, r);
  int power = 1;
  for (int i = 0; i < parity_binary.length(); i++) {
    data[power - 1] = parity_binary[i];
    power *= 2;
  }
  return data;
}

/**
 * @brief encodes message bits into hamming code
 * @param n length of codeword
 * @param k length of message bits
 * @param bits full message
 * @return encoded message
 */
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
    // cout << chunk << endl;
    ans += chunk;
  }
  return ans;
}

/**
 * @brief injects a single bit error inside the encoded bits
 * @param data encoded message
 * @param pos position of the error [0 based]
 * @return the bitstream with error
 */
string inject_single_bit_error(string data, unsigned int pos) {
  data[pos] = data[pos] == '1' ? '0' : '1';
  return data;
}

int main() {
  ClientConnection cc = ClientConnection();
  int n, k;
  puts("Enter n and k:");
  cin >> n >> k;

  puts("Enter bitstream:");
  string original_bits;
  cin >> original_bits;

  string encoded_bits = hamming_code(n, k, original_bits); // encode the message

  printf("Enter error postion: (0 for no error)\n");
  int err_pos = -1;
  cin >> err_pos;

  cout << "Encoded bits:             " << encoded_bits << endl; // before error
  if (err_pos - 1 >= 0) {
    encoded_bits = inject_single_bit_error(encoded_bits, err_pos - 1); // inject error
    cout << "Encoded bits after error: " << encoded_bits << endl;      // after error
  }

  string volts = manchest_encode(encoded_bits); // encode with manchester
  string msgToBeSent = volts + "schema=" + to_string(n) + "," + to_string(k); // concatenating n and k
  cc.sendMessage(msgToBeSent); // sending to server
}