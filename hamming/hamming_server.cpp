#include <bits/stdc++.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8080

using namespace std;

// struct for setting up connection
struct ServerConnection {
  int server_fd, new_socket, valread;
  struct sockaddr_in address;
  int opt = 1;
  int addrlen = sizeof(address);

  ServerConnection(int port = PORT) { setup_connection(port); }

  int setup_connection(int port) {
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
      perror("socket failed");
      exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
      perror("setsockopt");
      exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
      perror("bind failed");
      exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
      perror("listen");
      exit(EXIT_FAILURE);
    }
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                             (socklen_t *)&addrlen)) < 0) {
      perror("accept");
      exit(EXIT_FAILURE);
    }
    return 1;
  }

  /**
   *@brief recives the message from client
   */
  string receiveMessage() {
    string str = "";
    char buffer[65536] = {0};
    valread = read(new_socket, buffer, 65536);
    str = buffer;
    return str;
  }

  /**
   *@brief sends message to client
   *@param msg message to be sent
   */
  void sendMessage(string msg) {
    send(new_socket, msg.c_str(), msg.length(), 0);
  }
};

/**
 *@brief converts manchester schemed voltage level to bitstream
 *@param volts voltage levels e.g., +5v, -5v, ..
 */
string manchest_decode(string volts) {
  string bits = "";
  for (int i = 0; i < volts.length(); i += 10) {
    if (volts[i] == volts[i + 3]) return "wrong scheme!";
  }

  for (int i = 0; i < volts.length(); i += 10) {
    bits += (volts[i] == '+') ? "0" : "1";
  }
  return bits;
}

/**
 *@brief removes the bits in position which has power of 2 such as 1, 2, 4, 8
 *@param data the bitstream with hamming parity
 */
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

/**
 *@brief generates the hamming parity set but in decimal form
 *@param data bitstream
 */
int generate_parity(string data) {
  int parity = 0;
  for (int i = 0; i < data.length(); i++)
    if (data[i] == '1') parity ^= (i + 1);
  return parity;
}

/**
 *@brief Decodes the bitstream encoded with hamming code,
 * detects and corrects single bit error, but cannot
 * detect two bit or burst error. Returns the original bits
 *@param n length of bitstream with code
 *@param k length of databits or bits without code
 *@param encoded bitstream encoded with hamming code
 */
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
      cout << "Error found in " << (i + 1) << "-th chunk , position: " << parity
           << endl;
      cout << "Error corrected!\n";
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

int main() {
  ServerConnection sc = ServerConnection();  // setting up server
  int n, k;
  string rcvmsg = sc.receiveMessage();  // receive any message from client
  string volts = "";
  int code_index = -1, bit_index = -1;

  // extract the volt levels from received message
  for (int i = 0; i < rcvmsg.length(); i++) {
    if (rcvmsg.substr(i, 7) == "schema=") {
      code_index = i + 7;
      break;
    }
    volts += rcvmsg[i];
  }

  // extract the code length
  string code_len, bit_len;
  for (int i = code_index; i < rcvmsg.length(); i++) {
    if (rcvmsg[i] == ',') {
      bit_index = i + 1;
      break;
    }
    code_len += rcvmsg[i];
  }
  // extract the message length
  for (int i = bit_index; i < rcvmsg.length(); i++) {
    bit_len += rcvmsg[i];
  }
  n = stoi(code_len);
  k = stoi(bit_len);

  // decode the voltage
  string bits = manchest_decode(volts);
  // decode the bits
  string decoded_bits = hamming_decode(n, k, bits);
  cout << decoded_bits << endl;
}