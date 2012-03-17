
/*

Missing tests:
Yes -> I win

*/



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
    if (s.compare("Quit") == 0) {        // TODO
        return Quit;
    } else if (s.compare("Yes)") == 0) {
        return Yes;
    } else {
        return No;
    }
}


void say_goodbye()  {
    const char* goodbye = "Ok. Bye!";
    std::cout << goodbye << std::endl;
}

int main() {

    intro();
    ask_question();
    PossibleAnswers answer = get_answer();
    if (Quit == answer) {
        say_goodbye();
        return 0;
    }

    return 0;
}