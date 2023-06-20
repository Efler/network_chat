#include <iostream>
#include "server.hpp"


int main(){

    setlocale(LC_ALL, "ru");
    server chat_server(2000);
    chat_server.run();

    system("pause");

}