
/*

Missing tests:
Yes Yes -> two wins
Add "Think of an animal" on each round
No isn't necessarily the end of the game
*/



#include <iostream>
#include <assert.h>
#include <string>
#include <memory>

void intro()  {
    const char* intro =
"Hi, let's play the animal game. You can quit at any time by typing \"Quit.\"\n\
\n\
Think of an animal.";
    std::cout << intro << std::endl;
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
    if ((s.size() == 0) || (s == "Quit")) {
        return Quit;
    } else if (s == "Yes") {
        return Yes;
    } else {
        return No;
    }
}


class KnowledgeItem;
typedef std::shared_ptr<KnowledgeItem> KnowledgeItemPtr;

class KnowledgeItem {
public:
    KnowledgeItem(const std::string text)
        : text_(text)
    {}
    std::string getText() const {return text_;}
    virtual std::string getQuestion() const {
        return "Is your animal a " + getText() + "? ";
    }
	virtual bool toYesNode(const KnowledgeItemPtr& root, KnowledgeItemPtr& current) {
		current = root;
		return true;
	}
private:
    std::string text_;
};


class Question : public KnowledgeItem {
public:
    Question(const std::string text, KnowledgeItemPtr yes, KnowledgeItemPtr no)
        : KnowledgeItem(text)
        , yes_(yes)
        , no_(no)
    {}
    virtual std::string getQuestion() const {
        return getText() + " ";
    }
	virtual bool toYesNode(const KnowledgeItemPtr& root, KnowledgeItemPtr& current) {
		current = ((Question*)root.get())->yes_;
		return false;
	}
public: // TODO
    KnowledgeItemPtr yes_;
    KnowledgeItemPtr no_;
};

void ask_question(KnowledgeItemPtr animal)  {
    std::cout << animal->getQuestion();
}




void say_goodbye()  {
    const char* goodbye = "Ok. Bye!";
    std::cout << goodbye << std::endl;
}

void i_win()  {
    const char* win = "Ha! I win! Let's play again";
    std::cout << win << std::endl;
}

KnowledgeItemPtr i_loose(KnowledgeItemPtr current)  {
    const char* what_is_it = "Oh no. What was it? ";
    std::cout << what_is_it ;
    std::string newAnimalName = get_string(std::cin);
    KnowledgeItemPtr newAnimal(new KnowledgeItem(newAnimalName));

    std::cout << "What is a yes/no question to tell a " 
              << newAnimalName << " from a " 
              << current->getText() << "? ";
    std::string discriminate_new_animal = get_string(std::cin);

    KnowledgeItemPtr new_root(new Question(discriminate_new_animal, newAnimal, current));
  
    std::cout << "Thanks! Let's play again." << std::endl;
    return new_root;
}

int main() {

    KnowledgeItemPtr root(new KnowledgeItem("cat"));
    KnowledgeItemPtr current(root);

    intro();
    while (1) {
        ask_question(current);
        PossibleAnswers answer = get_answer();
        switch (answer) {
        case Quit:
            say_goodbye();
            return 0;
        case Yes:
			if (current->toYesNode(root, current)) {
				i_win();
			}
            break;
        case No:
            root = i_loose(current);
            current = root;
            break;
        default:
            assert(0);
        }
    }

    return 0;
}