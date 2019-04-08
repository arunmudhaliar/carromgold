//
//  NetworkManager.cpp
//  GDCPhysics
//
//  Created by Arun A on 02/11/18.
//  Copyright © 2018 Arun A. All rights reserved.
//

#include "NetworkManager.hpp"

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

void NetworkManager::on_open(client* c, websocketpp::connection_hdl hdl) {
//    std::string msg = "Hello";
//    c->send(hdl,msg,websocketpp::frame::opcode::text);
//    c->get_alog().write(websocketpp::log::alevel::app, "Sent Message: "+msg);
    NetworkManager::GetInstance().GetDelegate()->OnNetworkOpen();
}

void NetworkManager::on_fail(client* c, websocketpp::connection_hdl hdl) {
    c->get_alog().write(websocketpp::log::alevel::app, "Connection Failed");
    NetworkManager::GetInstance().GetDelegate()->OnNetworkFail();
}

void NetworkManager::on_close(client* c, websocketpp::connection_hdl hdl) {
    c->get_alog().write(websocketpp::log::alevel::app, "Connection Closed");
    NetworkManager::GetInstance().GetDelegate()->OnNetworkClose();
}

// This message handler will be invoked once for each incoming message. It
// prints the message and then sends a copy of the message back to the server.
void NetworkManager::on_message(client* c, websocketpp::connection_hdl hdl, message_ptr msg) {
    std::cout << "on_message called with hdl: " << hdl.lock().get()
    << " and message: " << msg->get_payload()
    << std::endl;
    
//    websocketpp::lib::error_code ec;
//    c->send(hdl, msg->get_payload(), msg->get_opcode(), ec);
//    if (ec) {
//        std::cout << "Echo failed because: " << ec.message() << std::endl;
//    }
    
    NetworkManager::GetInstance().GetDelegate()->OnNetworkMessage(msg->get_payload());
}

NetworkManager& NetworkManager::GetInstance() {
    static NetworkManager instance;
    return instance;
}

NetworkManager::NetworkManager() {
    this->delegate = nullptr;
}

NetworkManager::~NetworkManager() {
    c.stop();
}

void NetworkManager::SendMessage(const std::string& msg) {
    c.send(handle, msg, websocketpp::frame::opcode::text);
}

void NetworkManager::InitNetwork(NetworkManagerDelegate* delegate) {
    this->delegate = delegate;
//    std::string uri = "http://13.232.140.31:9094";
    std::string uri = "ws://127.0.0.1:3000";
    try {
        // Set logging to be pretty verbose (everything except message payloads)
        c.set_access_channels(websocketpp::log::alevel::all);
        c.clear_access_channels(websocketpp::log::alevel::frame_payload);
        
        // Initialize ASIO
        c.init_asio();
        
        // Register our message handler
        c.set_open_handler(bind(&NetworkManager::on_open,&c,::_1));
        c.set_fail_handler(bind(&NetworkManager::on_fail,&c,::_1));
        c.set_message_handler(bind(&NetworkManager::on_message,&c,::_1,::_2));
        c.set_close_handler(bind(&NetworkManager::on_close,&c,::_1));
        
        websocketpp::lib::error_code ec;
        client::connection_ptr con = c.get_connection(uri, ec);
        if (ec) {
            std::cout << "could not create connection because: " << ec.message() << std::endl;
            return;
        }
        
        handle = con->get_handle();
        
        // Note that connect here only requests a connection. No network messages are
        // exchanged until the event loop starts running in the next line.
        c.connect(con);
        
        // Start the ASIO io_service run loop
        // this will cause a single connection to be made to the server. c.run()
        // will exit when this connection is closed.
//        c.run();
        // Create a thread to run the ASIO io_service event loop
        websocketpp::lib::thread asio_thread(&client::run, &c);
        asio_thread.detach();
    } catch (websocketpp::exception const & e) {
        std::cout << e.what() << std::endl;
    }
}
