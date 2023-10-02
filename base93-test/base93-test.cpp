// base93-test.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.
//

#include <iostream>

#include "../Base93.hpp"

int main()
{
    str_t s;
    b93_t r, t;

    //1
    //IN
    s = "4seCoNds In$Space";
    
    std::cout << "IN DATA" << std::endl << "-------" << std::endl;
    std::cout << "ORIGINAL : " << s << std::endl;
    r = Base93::fromString(s);
    std::cout << "CONVERTED: " << r << std::endl;
    std::cout << std::endl;

    //OUT
    std::cout << "OUT DATA" << std::endl << "--------" << std::endl;
    t = Base93::toString(r);
    std::cout << "BACK    : " << t << std::endl;
    std::cout << std::endl;
}
