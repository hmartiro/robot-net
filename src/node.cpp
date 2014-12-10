/**
* Node class for distributed communication.
*/

#include <chrono>
#include <thread>
#include <vector>
#include <iostream>

#include "utils/ostreamlock.hpp"

#include "node.hpp"

using namespace std;

namespace rnet {

Node::Node(std::string remote_host, int pub_port, int sub_port, int req_port, int rep_port) :
    sock_pub(context, zmqpp::socket_type::publish),
    sock_sub(context, zmqpp::socket_type::subscribe),
//    sock_req(context, zmqpp::socket_type::request),
//    sock_rep(context, zmqpp::socket_type::reply),
    endpoint_pub("tcp://*:" + to_string(pub_port)),
    endpoint_sub("tcp://" + remote_host + ":" + to_string(sub_port)),
    endpoint_req("tcp://" + remote_host + ":" + to_string(req_port)),
    endpoint_rep("tcp://" + remote_host + ":" + to_string(rep_port)),
    to_exit(false)
{}

void Node::start() {

    // Launch a thread for each socket
    vector<thread> threads;
    threads.emplace_back(thread([this] { launch_pub_thread(); }));
    threads.emplace_back(thread([this] { launch_sub_thread(); }));
    threads.emplace_back(thread([this] { launch_req_thread(); }));
    threads.emplace_back(thread([this] { launch_rep_thread(); }));

    cout << oslock << "All threads launched." << endl << osunlock;

    // Block until all threads exit
    for(auto& t : threads) t.join();
}

void Node::launch_pub_thread() {

    cout << oslock << "Binding publish socket to [" << endpoint_pub << "]." << endl << osunlock;

    sock_pub.bind(endpoint_pub);

    // Pause to connect
    this_thread::sleep_for(chrono::milliseconds(1000));

    while(!to_exit) {
    }
}

void Node::launch_sub_thread() {

    cout << oslock << "Connecting subscribe socket to [" << endpoint_sub << "]." << endl << osunlock;

    sock_sub.connect(endpoint_sub);

    while(!to_exit) {
    }
}

void Node::launch_req_thread() {

    cout << oslock << "Connecting request socket to [" << endpoint_req << "]." << endl << osunlock;

    while(!to_exit) {
    }
}

void Node::launch_rep_thread() {

    cout << oslock << "Connecting reply socket to [" << endpoint_rep << "]." << endl << osunlock;

    while(!to_exit) {
    }
}

} // End namespace rnet
