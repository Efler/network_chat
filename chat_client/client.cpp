#include <iostream>
#include "client.hpp"


int main() {

    setlocale(LC_ALL, "ru");

    sf::IpAddress ip = sf::IpAddress::getLocalAddress();
    string ip_str = ip.toString();

    client chat_client;
    chat_client.connect(ip_str, 2000);

    system("pause");

}