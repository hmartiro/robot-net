/**
* Example of basic usage of the Node class.
*/

#include <string>
#include <thread>
#include <unistd.h>

#include "../src/node.hpp"

using namespace std;

int main(int argc, char *argv[]) {

  string host = "localhost";
  int port = 4240;

  // Create a node to connect to this location
  rnet::Node node_A = {
      host,
      port, port + 1, port + 2, port + 3
  };

  rnet::Node node_B = {
      host,
      port + 1, port, port + 3, port + 2
  };

  thread node_A_thread = thread([&node_A] {
    node_A.start();
  });

  thread node_B_thread = thread([&node_B] {
    node_B.start();
  });

  sleep(1);

  node_A.publish("Hello from node A!");
  node_B.publish("Hello from node B!");

  node_A_thread.join();
  node_B_thread.join();

  return 0;
}
