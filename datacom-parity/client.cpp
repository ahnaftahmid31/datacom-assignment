// Client side C/C++ program to demonstrate Socket programming
#include <arpa/inet.h>
#include <bits/stdc++.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8080

using namespace std;

struct ClientConnection {
  int sock = 0, valread;
  struct sockaddr_in serv_addr;

  char buffer[1024] = {0};

  ClientConnection(int port = PORT) {
    setup_connection(port);
    receive_initial_message();
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

  void receive_initial_message() {
    cout << receiveMessage() << endl;
  }
};

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
  // for (int i = 0; i < ans.length(); i++) {
  //   printf("%c", ans[i]);
  //   if ((i + 1) % (aschii_length + 1) == 0) puts("");
  // }
  return ans;
}

int main() {
  string str = "";

  ClientConnection cc = ClientConnection();

  while (true) {
    getline(cin, str);
    if (str == "end" || str == "End") break;
    if (str.length() % (aschii_length * 4) != 0) {
      printf("[Client]: Please Give bitstream with length of %d\n", aschii_length * 4);
      continue;
    }
    str = highdense_encode(two_dimensional_parity_encode(str));
    cc.sendMessage(str);
    puts("");
    cout << cc.receiveMessage() << endl;
  }
  return 0;
}
