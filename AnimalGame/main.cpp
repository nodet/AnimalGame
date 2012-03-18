#include <iostream>
#include <assert.h>
#include <string>
#include <memory>





class Messenger {
public:
    typedef std::shared_ptr<Messenger> Ptr;

    virtual std::string get_string(std::istream& s) = 0;

    virtual void say_hello() = 0;
    virtual void say_goodbye() = 0;
    virtual void say_think() = 0;
    virtual void say_play_again() = 0;
    virtual void say_i_win() = 0;
    virtual std::string guess_animal(const std::string& animalName) = 0;
    virtual std::string ask_yes_no_question(std::string newAnimalName, std::string badName) = 0;
    virtual std::string ask_new_animal_name() = 0;

    virtual const char* yes() = 0;
    virtual const char* quit() = 0;
};

class Cin_Cout_Messenger : public Messenger {
    // All input/output gathered as static methods of this class
    // intended to be a template parameter of the others.

    // Individual messages could be made static members of some other class
    // to further decouple the language from the fact that we use cin/cout.
public:
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
    void say_hello()  {
        const char* intro = "Hi, let's play the animal game. You can quit at any time by typing \"Quit.\"\n";
        std::cout << intro << std::endl;
    }
    void say_goodbye()  {
        const char* goodbye = "Ok. Bye!";
        std::cout << goodbye << std::endl;
    }
    void say_think()  {
        std::cout << "Think of an animal." << std::endl;
    }
    void say_play_again() {
        std::cout << "Thanks! Let's play again." << std::endl;
    }
    void say_i_win() {
        const char* win = "Ha! I win! Let's play again";
        std::cout << win << std::endl;
    }
    std::string guess_animal(const std::string& animalName) {
        return "Is your animal a " + animalName + "? ";
    }
    std::string ask_yes_no_question(std::string newAnimalName, std::string badName) {
        std::cout << "What is a yes/no question to tell a " + newAnimalName + " from a " + badName + "? ";
        return get_string(std::cin);
    }
    std::string ask_new_animal_name() {
        std::cout << "Oh no. What was it? ";
        return get_string(std::cin);
    }

    const char* yes() {return "Yes";}
    const char* quit() {return "Quit";}

};



class KnowledgeItem {
public:
    typedef std::shared_ptr<KnowledgeItem> Ptr;
    enum PossibleAnswers {
        Yes, No, Quit
    };
    virtual PossibleAnswers ask_question(Messenger::Ptr messenger) const = 0;

	virtual bool toYesNode(Messenger::Ptr messenger, const Ptr& root, Ptr& current) = 0;
    virtual void toNoNode(Messenger::Ptr messenger, Ptr& root, Ptr& current) = 0;

    virtual std::string getText() const = 0;

};


class Animal : public KnowledgeItem {
public:
    Animal(const std::string text)
        : text_(text)
    {}
    virtual PossibleAnswers ask_question(Messenger::Ptr messenger) const {
        std::cout << get_question_to_ask(messenger);
        return get_answer(messenger);
    }
    virtual bool toYesNode(Messenger::Ptr messenger, const Ptr& root, Ptr& current);
    virtual void toNoNode(Messenger::Ptr messenger, Ptr& root, Ptr& current);

protected:
    virtual std::string getText() const {return text_;}
    virtual std::string get_question_to_ask(Messenger::Ptr messenger) const {
        return messenger->guess_animal(getText());
    }

private:
    static PossibleAnswers get_answer(Messenger::Ptr messenger) {
        std::string s = messenger->get_string(std::cin);
        if ((s.size() == 0) || (s == messenger->quit())) {
            return Quit;
        } else if (s == messenger->yes()) {
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
    Question(const std::string text, Ptr yes, Ptr no)
        : Animal(text)
        , yes_(yes)
        , no_(no)
    {}
	virtual bool toYesNode(Messenger::Ptr messenger, const Ptr& root, Ptr& current);
    virtual void toNoNode(Messenger::Ptr messenger, Ptr& root, Ptr& current);

protected:
    virtual std::string get_question_to_ask(Messenger::Ptr messenger) const {
        return getText() + " ";
    }

private:
    Ptr yes_;
    Ptr no_;
};


void Animal::toNoNode(Messenger::Ptr messenger, Ptr& root, Ptr& current) {
    std::string newAnimalName = messenger->ask_new_animal_name();
    Ptr newAnimal(new Animal(newAnimalName));

    std::string discriminate_new_animal = messenger->ask_yes_no_question(newAnimalName, current->getText());

    Ptr new_root(new Question(discriminate_new_animal, newAnimal, current));
    root = new_root;
    current = root;

    messenger->say_play_again();
    messenger->say_think();
}


bool Animal::toYesNode(Messenger::Ptr messenger, const Ptr& root, Ptr& current) {
    current = root;
    messenger->say_i_win();
    messenger->say_think();
    return true;
}

void Question::toNoNode(Messenger::Ptr messenger, Ptr& root, Ptr& current) {
    current = no_;
}

bool Question::toYesNode(Messenger::Ptr messenger, const Ptr& root, Ptr& current) {
    current = ((Question*)root.get())->yes_;
    return false;
}


void run(Messenger::Ptr messenger) {
    KnowledgeItem::Ptr root(new Animal("cat"));
    KnowledgeItem::Ptr current(root);

    messenger->say_hello();
    messenger->say_think();  // TODO: it's odd that this only appears once...  
                             // Others are inside toYesNode or toNoNode...
    while (1) {
        switch (current->ask_question(messenger)) {
        case KnowledgeItem::Quit:
            messenger->say_goodbye();
            return;
        case KnowledgeItem::Yes:
            current->toYesNode(messenger, root, current);
            break;
        case KnowledgeItem::No:
            current->toNoNode(messenger, root, current);
            break;
        default:
            assert(0);
        }
    }

}



int main() {

    Messenger::Ptr messenger (new Cin_Cout_Messenger);
    run(messenger);
    return 0;
}