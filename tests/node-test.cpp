/**
* Example of basic usage of the Node class.
*/

#include <string>

#include "../src/node.hpp"

using namespace std;

int main(int argc, char *argv[]) {

  string host = "localhost";
  int port = 4240;

  // Create a node to connect to this location
  rnet::Node node = {
      host,
      port, port + 1, port + 2, port + 3
  };

  // Start all sockets of the node
  node.start();

  return 0;
}
