#ifndef CHAT_SERVER_SERVER_HPP
#define CHAT_SERVER_SERVER_HPP
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <SFML/Network.hpp>
using namespace std;


class server final{

private:

    unsigned long _port;
    sf::TcpListener* _listener;
    vector<pair<sf::TcpSocket*, string>> _clients;
    bool _is_running;

public:

    explicit server(unsigned long port):
        _is_running(false), _listener(new sf::TcpListener), _port(port){}

    server(const server& other)              = delete;
    server(server&& other)                   = delete;
    server& operator = (const server& other) = delete;
    server& operator = (server&& other)      = delete;

    ~server(){
        while(!_clients.empty()){
            pair<sf::TcpSocket*, string> client = _clients[_clients.size() - 1];
            cout << "[Destructor: disconnecting '" << get<1>(client) << "']" <<  endl;
            delete get<0>(client);
        }
        _listener->close();
        delete _listener;
    }

public:

    void run(){
        cout << "\n### Starting Server ###\n" << endl;
        _is_running = true;
        if(_listener->listen(_port) != sf::Socket::Done){
            _is_running = false;
            cout << "[Listener could not listen port " << _port << "!]" << endl;
        }

        while(_is_running){

            thread connection_thread(&server::connect_clients, this);
            connection_thread.detach();

            manage_packets();

        }

        _listener->close();
    }

private:

    void connect_clients(){

        while(_is_running){
            auto* new_client = new sf::TcpSocket;
            if(_listener->accept(*new_client) == sf::Socket::Done){
                new_client->setBlocking(true);
                string client_name;
                sf::Packet packet;
                new_client->receive(packet);
                new_client->setBlocking(false);
                packet >> client_name;
                packet.clear();
                _clients.emplace_back(new_client, client_name);
                string log =  "[Client '" + client_name + "' connected ";
                string additional_log = "(total clients on server: " + to_string(_clients.size()) + ")";
                log += additional_log;
                log += "]";
                packet << log;
                send_packet(packet, _clients.size() - 1);
                packet.clear();
                packet << additional_log;
                new_client->send(packet);
                packet.clear();
                cout << log << endl;
            }
            else{
                if(_is_running) cout << "[Server listener connection error!]" << endl;
                delete new_client;
            }
        }

    }

    void manage_packets(){

        while(_is_running){

            for (size_t it = 0; it < _clients.size(); ++it){
                receive_packet(it);
            }

            std::this_thread::sleep_for((std::chrono::milliseconds)250);
        }

    }

    void receive_packet(size_t client_id){
        auto client = _clients[client_id];
        sf::Packet packet;
        auto status = get<0>(client)->receive(packet);

        if (status == sf::Socket::Disconnected){
            disconnect_client(client_id);
            return;
        }else if(status == sf::Socket::Done){
            if (packet.getDataSize() > 0)
            {
                string received_message;
                packet >> received_message;
                packet.clear();

                packet << ("[" + get<1>(client) + "]: " + received_message);

                send_packet(packet, client_id);
                cout << "[" << (get<1>(client) + "]: " + received_message) << endl;
                packet.clear();
            }
        }
    }

    void disconnect_client(size_t client_id){

        auto client = _clients[client_id];
        string log = "[Client '" + get<1>(client) + "' disconnected]";
        sf::Packet packet;
        packet << log;
        send_packet(packet, client_id);
        packet.clear();
        delete get<0>(client);
        _clients.erase(_clients.begin() + (long long)client_id);
        cout << log << endl;

        if(_clients.empty()){
            cout << "\nAll clients disconnected, stopping server!" << endl;
            _is_running = false;
        }
    }

    void send_packet(sf::Packet& packet, size_t client_id){

        for(size_t iter = 0; iter < _clients.size(); ++iter){
            if(iter != client_id){
                auto* client = get<0>(_clients[iter]);
                if(client->send(packet) != sf::Socket::Done){
                    cout << "[Could not send packet to client '" << get<1>(_clients[iter]) << "'!]" << endl;
                }
            }
        }

    }

};


#endif //CHAT_SERVER_SERVER_HPP