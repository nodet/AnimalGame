


#include <iostream>


void intro()  {
    const char* intro =
"Hi, let's play the animal game. You can quit at any time by typing \"Quit.\"\n\
\n\
Think of an animal. \n";
    std::cout << intro << std::endl;
}

int main() {

    intro();

    return 0;
}