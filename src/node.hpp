/**
* Node class for distributed communication.
*/

#pragma once

#include <string>
#include <zmqpp/zmqpp.hpp>

namespace rnet {

class Node {

public:

  /**
  * Create from a single remote host and unique ports for each socket.
  */
  Node(std::string remote_host, int pub_port, int sub_port, int req_port, int rep_port);

  /**
  * Start the node by connecting the sockets and launching threads.
  */
  void start();

private:

  // One thread to handle each socket
  void launch_pub_thread();
  void launch_sub_thread();
  void launch_req_thread();
  void launch_rep_thread();

  // Context for all ZMQ sockets
  zmqpp::context context;

  // Publish/subscribe sockets for fast data transmission
  zmqpp::socket sock_pub, sock_sub;

  // Request/reply sockets for configuration
  //zmqpp::socket sock_req, sock_rep;

  // Endpoints for all sockets
  std::string endpoint_pub, endpoint_sub;
  std::string endpoint_req, endpoint_rep;

  // Flag to exit everything
  bool to_exit;
};

} // End namespace rnet
