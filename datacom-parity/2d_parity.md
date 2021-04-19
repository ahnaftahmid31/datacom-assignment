## How to run this project?

- first compile the server.cpp with
```
  g++ server.cpp -o server && ./server
```

- then compile the client.cpp with
```
g++ client.cpp -o client && ./client
```

## What input we have to give?

Assuming we will be sending ascii characters, we will need to give at least 4 of ascii characters to give it a 2D parity check system. So we need to give multiple of 32 length bit pattern.
For example:
```
10011001111000100010010010000100
```

## Does it support reading from file ?

No it doesn't.