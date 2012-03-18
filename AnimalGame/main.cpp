
/*

Missing tests:
Yes Yes -> two wins
No isn't necessarily the end of the game
*/



#include <iostream>
#include <assert.h>
#include <string>
#include <memory>




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


class KnowledgeItem;
typedef std::shared_ptr<KnowledgeItem> KnowledgeItemPtr;

class KnowledgeItem {
public:
    enum PossibleAnswers {
        Yes, No, Quit
    };
    virtual PossibleAnswers ask_question() const = 0;

	virtual bool toYesNode(const KnowledgeItemPtr& root, KnowledgeItemPtr& current) = 0;
    virtual void toNoNode(KnowledgeItemPtr& root, KnowledgeItemPtr& current) = 0;

    virtual std::string getText() const = 0;

};


class Animal : public KnowledgeItem {
public:
    Animal(const std::string text)
        : text_(text)
    {}
    virtual PossibleAnswers ask_question() const {
        std::cout << getQuestion();
        return get_answer();
    }
    virtual bool toYesNode(const KnowledgeItemPtr& root, KnowledgeItemPtr& current);
    virtual void toNoNode(KnowledgeItemPtr& root, KnowledgeItemPtr& current);

protected:
    virtual std::string getText() const {return text_;}
    virtual std::string getQuestion() const {
        return "Is your animal a " + getText() + "? ";
    }

private:
    static PossibleAnswers get_answer() {
        std::string s = get_string(std::cin);
        if ((s.size() == 0) || (s == "Quit")) {
            return Quit;
        } else if (s == "Yes") {
            return Yes;
        } else {
            return No;
        }
    }

private:
    std::string text_;
};


class Question : public Animal {
public:
    Question(const std::string text, KnowledgeItemPtr yes, KnowledgeItemPtr no)
        : Animal(text)
        , yes_(yes)
        , no_(no)
    {}
	virtual bool toYesNode(const KnowledgeItemPtr& root, KnowledgeItemPtr& current);
    virtual void toNoNode(KnowledgeItemPtr& root, KnowledgeItemPtr& current);

protected:
    virtual std::string getQuestion() const {
        return getText() + " ";
    }

private:
    KnowledgeItemPtr yes_;
    KnowledgeItemPtr no_;
};



void Animal::toNoNode(KnowledgeItemPtr& root, KnowledgeItemPtr& current) {
    const char* what_is_it = "Oh no. What was it? ";
    std::cout << what_is_it ;
    std::string newAnimalName = get_string(std::cin);
    KnowledgeItemPtr newAnimal(new Animal(newAnimalName));

    std::cout << "What is a yes/no question to tell a " 
        << newAnimalName << " from a " 
        << current->getText() << "? ";
    std::string discriminate_new_animal = get_string(std::cin);

    KnowledgeItemPtr new_root(new Question(discriminate_new_animal, newAnimal, current));
    root = new_root;
    current = root;

    std::cout << "Thanks! Let's play again." << std::endl;
    std::cout << "Think of an animal." << std::endl;
}


bool Animal::toYesNode(const KnowledgeItemPtr& root, KnowledgeItemPtr& current) {
    current = root;
    const char* win = "Ha! I win! Let's play again";
    std::cout << win << std::endl;
    std::cout << "Think of an animal." << std::endl;
    return true;
}

void Question::toNoNode(KnowledgeItemPtr& root, KnowledgeItemPtr& current) {
    current = no_;
}

bool Question::toYesNode(const KnowledgeItemPtr& root, KnowledgeItemPtr& current) {
    current = ((Question*)root.get())->yes_;
    return false;
}




void intro()  {
    const char* intro =
"Hi, let's play the animal game. You can quit at any time by typing \"Quit.\"\n\
\n\
Think of an animal.";
    std::cout << intro << std::endl;
}

void say_goodbye()  {
    const char* goodbye = "Ok. Bye!";
    std::cout << goodbye << std::endl;
}

int main() {

    KnowledgeItemPtr root(new Animal("cat"));
    KnowledgeItemPtr current(root);

    intro();
    while (1) {
        switch (current->ask_question()) {
        case KnowledgeItem::Quit:
            say_goodbye();
            return 0;
        case KnowledgeItem::Yes:
			current->toYesNode(root, current);
            break;
        case KnowledgeItem::No:
            current->toNoNode(root, current);
            break;
        default:
            assert(0);
        }
    }

    return 0;
}