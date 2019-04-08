//
//  NetworkManager.hpp
//  GDCPhysics
//
//  Created by Arun A on 02/11/18.
//  Copyright © 2018 Arun A. All rights reserved.
//

#ifndef NetworkManager_hpp
#define NetworkManager_hpp

#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <iostream>

typedef websocketpp::client<websocketpp::config::asio_client> client;
// pull out the type of messages sent by our config
typedef websocketpp::config::asio_client::message_type::ptr message_ptr;

class NetworkManagerDelegate {
public:
    virtual void OnNetworkMessage(const std::string& msg) = 0;
    virtual void OnNetworkFail() = 0;
    virtual void OnNetworkOpen() = 0;
    virtual void OnNetworkClose() = 0;
};

class NetworkManager {
private:
    NetworkManager();
    
public:
    ~NetworkManager();
    
    void InitNetwork(NetworkManagerDelegate* delegate);
    static NetworkManager& GetInstance();
    
    inline NetworkManagerDelegate* GetDelegate() { return delegate; }
    
    void SendMessage(const std::string& msg);
    
private:
    client c;
    websocketpp::connection_hdl handle;
    NetworkManagerDelegate* delegate;
    
    static void on_open(client* c, websocketpp::connection_hdl hdl);
    static void on_fail(client* c, websocketpp::connection_hdl hdl);
    static void on_close(client* c, websocketpp::connection_hdl hdl);
    static void on_message(client* c, websocketpp::connection_hdl hdl, message_ptr msg);
};

#endif /* Connection_hpp */
