


#include <iostream>


void intro()  {
    const char* intro =
"Hi, let's play the animal game. You can quit at any time by typing \"Quit.\"\n\
\n\
Think of an animal.";
    std::cout << intro << std::endl;
}



void ask_question()  {
    const char* is_your_animal = "Is your animal a ";
    std::cout << is_your_animal << "cat" << "? ";
}


int main() {

    intro();
    ask_question();

    return 0;
}