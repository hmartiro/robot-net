/**
* Example of ZeroMQ pub/sub usage for C++11.
*/

#include <zmqpp/zmqpp.hpp>
#include <iostream>
#include <chrono>
#include <thread>

using namespace std;

static const string PUBLISH_ENDPOINT = "tcp://*:4242";

int main(int argc, char *argv[]) {

  // Create a publisher socket
  zmqpp::context context;
  zmqpp::socket_type type = zmqpp::socket_type::pair;
  zmqpp::socket socket (context, type);

  // Open the connection
  cout << "Binding to " << PUBLISH_ENDPOINT << "..." << endl;
  socket.bind(PUBLISH_ENDPOINT);

  // Pause to connect
  this_thread::sleep_for(chrono::milliseconds(1000));

  // Number of messages sent
  int count = 0;

  while(true) {

    // Current time in ms
    unsigned long ms = chrono::system_clock::now().time_since_epoch() /
        chrono::milliseconds(1);

    string text = "Hello at " + to_string(ms);

    // Create a message and feed data into it
    zmqpp::message message;
    message << text;

    // Send it off to any subscribers
    socket.send(message);
    count++;

    if(count % 10000 == 0)
      cout << "[SENT] #" << count << " at " << ms << ": " << text << endl;

    //this_thread::sleep_for(chrono::microseconds(10));
  }

  // Unreachable, but for good measure
  socket.disconnect(PUBLISH_ENDPOINT);
  return 0;
}
