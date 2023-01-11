#include <iostream>

int PressAnyKey( const char* prompt )
{
    std::cout << (prompt ? prompt : "Press any button: ");
    std::cin.ignore();
    std::cin.get();
}