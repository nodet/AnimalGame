
/*

Missing tests:
Yes -> I win
Yes Yes -> two wins
*/



#include <iostream>
#include <assert.h>

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


enum PossibleAnswers {
    Yes, No, Quit
};


std::string get_string(std::istream& s) {
    std::string result;
    char c;
    while (s.get(c)) {
        if ('\n' == c) {
            return result;
        }
        result += c;
    }
    return result;
}

PossibleAnswers get_answer() {
    std::string s = get_string(std::cin);  // TODO
    if ((s.size() == 0) || (s.compare("Quit") == 0)) {        // TODO
        return Quit;
    } else if (s.compare("Yes") == 0) {
        return Yes;
    } else {
        return No;
    }
}


void say_goodbye()  {
    const char* goodbye = "Ok. Bye!";
    std::cout << goodbye << std::endl;
}

void i_win()  {
    const char* win = "Ha! I win! Let's play again";
    std::cout << win << std::endl;
}

void i_loose()  {
    const char* what_is_it = "Oh no. What was it? ";
    std::cout << what_is_it ;
    std::string newAnimal = get_string(std::cin);

    const char* discriminate = "What is a yes/no question to tell a lobster from a cat? ";
    std::cout << discriminate ;
    std::string discriminate_new_animal = get_string(std::cin);
    std::cout << "Thanks! Let's play again." << std::endl;
}

int main() {

    intro();
    while (1) {
        ask_question();
        PossibleAnswers answer = get_answer();
        switch (answer) {
        case Quit:
            say_goodbye();
            return 0;
        case Yes:
            i_win();
            break;
        case No:
            i_loose();
            break;
        default:
            assert(0);
        }
    }

    return 0;
}