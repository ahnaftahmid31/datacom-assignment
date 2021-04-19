// Server side C/C++ program to demonstrate Socket programming
#include <bits/stdc++.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8080

using namespace std;

struct ServerConnection {
  int server_fd,
      new_socket,
      valread;
  struct sockaddr_in address;
  int opt = 1;
  int addrlen = sizeof(address);

  ServerConnection(int port = PORT) {
    setup_connection(port);
    send_initial_message();
  }

  int setup_connection(int port) {
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
      perror("socket failed");
      exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
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

  void send_initial_message() {
    string str = "[Server]: Welcome! I'll accept ASCII bit stream\nencoded as HDB3 (+5, +0, +5, ...) and decode it \nbits length should be multiple of 32\nSend \"End\" to disconnect!\n";
    sendMessage(str);
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

int aschii_length = 8;
string err_msg;
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
  if (bits[0] != '0' && bits[0] != '1') return "Nothing";
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
          err_msg += "Error: row parity error at row ";
          err_msg += to_string(j + 1);
          err_msg += "\n";
        }
      }

      // checking columns (ignoring last column cz that's parity col)
      for (int j = 0; j < aschii_length; j++) {
        if (col_check[j] % 2 != (col_par[j] - '0')) {
          col_err[j] = true;
          has_error = true;
          printf("error found at col %d\n", j + 1);
          err_msg += "Error: row parity error at column ";
          err_msg += to_string(j + 1);
          err_msg += "\n";
        }
      }

      // correct if possible
      if (has_error) {
        for (int j = 0; j < 4; j++) {
          for (int k = 0; k < aschii_length; k++) {
            if (row_err[j] && col_err[k]) {
              printf("correcting error at %d row and %d column", j + 1, k + 1);
              err_msg += "Fix Error: at row ";
              err_msg += to_string(j + 1);
              err_msg += " Fix Error: at column ";
              err_msg += to_string(k + 1);
              err_msg += "\n";
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

// handle input from client and process them
void keep_getting_messages(ServerConnection sc) {
  while (true) {
    string rcv = sc.receiveMessage();
    if (rcv == "end" || rcv == "End") break;

    // decode volt levels with HDB3 decoder
    err_msg = "";
    string bits = highdense_decode(rcv);
    if (bits[0] != '0' && bits[0] != '1') {
      err_msg += "[Server]: ";
      err_msg += "Error in HDB3 decode!:\n";
      err_msg += bits;
      err_msg += "\n";
    }

    // decode 2D parity
    string decoded_bits = two_dimensional_parity_decode(bits);
    string msgToSend = "";

    if (err_msg.length() > 0) {
      err_msg += "[Server]: ";
      msgToSend += err_msg;
    }
    msgToSend += "[Server]: Decoded Bit pattern: \n";
    msgToSend += decoded_bits;
    msgToSend += "\n";
    sc.sendMessage(msgToSend);
  }
}

int main() {
  ServerConnection sc = ServerConnection();
  keep_getting_messages(sc);
  return 0;
}