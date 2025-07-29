#include <iostream>
#include "json.cpp"

int main(int argc, char * argv[]) {
    if (argc != 2) return EXIT_FAILURE;
    std::string json = JSON(argv[1]);
    if (json == "") return EXIT_FAILURE;
    std::cout << json;
    return EXIT_SUCCESS;
}