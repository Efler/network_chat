#ifndef CHAT_CLIENT_CLIENT_HPP
#define CHAT_CLIENT_CLIENT_HPP
#include <iostream>
#include <thread>
#include <chrono>
#include <SFML/Network.hpp>
using namespace std;


class client final{

private:

    sf::TcpSocket* _server;
    string _nickname;
    bool _is_connected;

public:

    client(): _server(new sf::TcpSocket), _nickname("unknown"), _is_connected(false)
    {
        cout << "\nEnter a nickname: " << endl;
        getline(cin, _nickname);
    }

    client(const client& other)              = delete;
    client(client&& other)                   = delete;
    client& operator = (const client& other) = delete;
    client& operator = (client&& other)      = delete;

    ~client(){
        _server->disconnect();
        delete _server;
    }

public:

    void connect(const string& ip, unsigned long port){
        sf::IpAddress addr = ip;
        if(_server->connect(addr, port) != sf::Socket::Done){
            cout << "Connection failed!" << endl;
            return;
        }else{
            cout << "Connection successful!" << endl;
            cout << "Type '/disconnect' for disconnection from server!\n" << endl;
            _is_connected = true;
            _server->setBlocking(false);
            sf::Packet packet;
            packet << _nickname;
            _server->send(packet);
            packet.clear();

            thread receive_thread(&client::receive_packets, this);
            receive_thread.detach();

            while(_is_connected){
                string message;
                getline(cin, message);

                if(message.empty())
                    continue;
                else if(message == "/disconnect"){
                    _server->disconnect();
                    _is_connected = false;
                    break;
                }

                sf::Packet reply_packet;
                reply_packet << message;

                send_packet(reply_packet);
                reply_packet.clear();
            }

            cout << "\n[Disconnected from the server!]" << endl;
        }
    }

private:

    void receive_packets(){
        sf::Packet packet;
        while(_is_connected){
            if (_server->receive(packet) == sf::Socket::Done){
                string received_string;
                packet >> received_string;
                cout << received_string << endl;
                packet.clear();
            }

            std::this_thread::sleep_for((std::chrono::milliseconds)250);
        }
    }

    void send_packet(sf::Packet packet){
        if (packet.getDataSize() > 0 && _server->send(packet) != sf::Socket::Done){
            cout << "[Packet sending failed!]" << endl;
        }
    }

};


#endif //CHAT_CLIENT_CLIENT_HPP