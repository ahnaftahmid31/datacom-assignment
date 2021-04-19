#include <bits/stdc++.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8080

using namespace std;

// struct for setting up connection
struct ServerConnection {
  int server_fd,
      new_socket,
      valread;
  struct sockaddr_in address;
  int opt = 1;
  int addrlen = sizeof(address);

  ServerConnection(int port = PORT) {
    setup_connection(port);
  }

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
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
      perror("accept");
      exit(EXIT_FAILURE);
    }
    return 1;
  }

  string receiveMessage() {
    string str = "";
    char buffer[65536] = {0};
    valread = read(new_socket, buffer, 65536);
    str = buffer;
    return str;
  }

  void sendMessage(string msg) {
    send(new_socket, msg.c_str(), msg.length(), 0);
  }
};

// decode manchester scheme
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

// x + 1
const vector<int> single_bit_poly{0, 1};
// x^7 + x^6 + 1
const vector<int> two_isolated_bit_poly{0, 1, 6};
// x^4 + x^2 + x + 1
const vector<int> odd_error_poly{0, 2, 1, 1};
// x^16 + x^12 + x^5 + 1 = (x + 1)(x^15 + x + 1)
const vector<int> burst_error_poly{0, 4, 7, 5};


// divide the bitstream received with the generator poly
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

// select a polynomial from single bit, 2 isolated, odd error, burst error
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

int main() {
  ServerConnection sc = ServerConnection(3001);

  while (true) {
    string msg = sc.receiveMessage();               // receive all message from client
    string volts = msg.substr(0, msg.length() - 1); // extract volts level from message
    string poly = msg.substr(msg.length() - 1, 1);  // extract polynomial type from message

    // convert string to int
    int poly_type = stoi(poly);
    if (poly_type > 4) break;
    printf("\npoly type: %d\n", poly_type);

    // decode the volts into bitstream
    string bits = manchest_decode(volts);
    cout << "Received bitstream:" << endl;
    cout << bits << endl;

    // divide the bitstream with generator
    string rem = crcdecode(bits, select_poly(poly_type));
    cout << "\nRemainder after decode:" << endl;
    cout << rem << endl;

    // if error found print
    for (int i = 0; i < rem.length(); i++) {
      if (rem[i] == '1') {
        puts("Error in the code, some bits were changed!\n");
        break;
      }
    }
  }
}