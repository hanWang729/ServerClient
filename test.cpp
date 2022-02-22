#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
int main() {
    int port = atoi(getenv("SOCKETPORT"));
    std::cout << port << std::endl;
    return 0;
}