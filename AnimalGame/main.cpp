#include <iostream>
#include <fstream>
#include <assert.h>
#include <string>
#include <memory>


class Messenger {
public:
    typedef std::shared_ptr<Messenger> Ptr;

    // TODO: All methods should probably be const
    virtual std::string get_string() = 0;

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

    // Non-const: would probably change some members if not using global streams
    virtual void say(std::string s) = 0;
};

class Cin_Cout_Messenger : public Messenger {
    // Individual messages could be made static members of some other class
    // to further decouple the language from the fact that we use cin/cout.
public:
    Cin_Cout_Messenger(std::istream& input = std::cin)
        : input_(input)
    {}
    std::string get_string() {
        std::string result;
        char c;
        while (input_.get(c)) {
            if ('\n' == c) {
                return result;
            }
            result += c;
        }
        return result;
    }
    void say_hello()  {
        const char* intro = "Hi, let's play the animal game. You can quit at any time by typing \"Quit.\"\n\n";
        say(intro);
    }
    void say_goodbye()  {
        const char* goodbye = "Ok. Bye!\n";
        say(goodbye);
    }
    void say_think()  {
        say("Think of an animal.\n");
    }
    void say_play_again() {
        say("Thanks! Let's play again.\n");
    }
    void say_i_win() {
        const char* win = "Ha! I win! Let's play again\n";
        say(win);
    }
    std::string guess_animal(const std::string& animalName) {
        return "Is your animal a " + animalName + "? ";
    }
    std::string ask_yes_no_question(std::string newAnimalName, std::string badName) {
        say("What is a yes/no question to tell a " + newAnimalName + " from a " + badName + "? ");
        return get_string();
    }
    std::string ask_new_animal_name() {
        say("Oh no. What was it? ");
        return get_string();
    }

    const char* yes() {return "Yes";}
    const char* quit() {return "Quit";}

    virtual void say(std::string s) {
        std::cout << s;
    }
private:
    std::istream& input_;
};



class KnowledgeItem {
public:
    typedef std::shared_ptr<KnowledgeItem> Ptr;
    enum PossibleAnswers {
        Yes, No, Quit
    };
    virtual PossibleAnswers ask_question(Messenger::Ptr messenger) const = 0;

	virtual bool toYesNode(Messenger::Ptr messenger, const Ptr& root, Ptr*& previous, Ptr& current) = 0;
    virtual void toNoNode(Messenger::Ptr messenger, const Ptr& root, Ptr*& previous, Ptr& current) = 0;

    virtual std::string getText() const = 0;

    virtual void display_tree(std::ofstream& file, bool first) = 0;
    virtual const KnowledgeItem* getRightMostAnimal(KnowledgeItem::Ptr*& toReset) = 0;
};


class Animal : public KnowledgeItem {
public:
    Animal(const std::string text)
        : text_(text)
    {}
    virtual PossibleAnswers ask_question(Messenger::Ptr messenger) const {
        messenger->say(get_question_to_ask(messenger));
        return get_answer(messenger);
    }
    virtual bool toYesNode(Messenger::Ptr messenger, const Ptr& root, Ptr*& previous, Ptr& current);
    virtual void toNoNode(Messenger::Ptr messenger, const Ptr& root, Ptr*& previous, Ptr& current);
    
    virtual void display_tree(std::ofstream&, bool) {
        // Intentionally blank
    }
    virtual const KnowledgeItem* getRightMostAnimal(KnowledgeItem::Ptr*& toReset) {
        return this;
    }

protected:
    virtual std::string getText() const {return text_;}
    virtual std::string get_question_to_ask(Messenger::Ptr messenger) const {
        return messenger->guess_animal(getText());
    }

private:
    static PossibleAnswers get_answer(Messenger::Ptr messenger) {
        std::string s = messenger->get_string();
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
	virtual bool toYesNode(Messenger::Ptr messenger, const Ptr& root, Ptr*& previous, Ptr& current);
    virtual void toNoNode(Messenger::Ptr messenger, const Ptr& root, Ptr*& previous, Ptr& current);

    virtual void display_tree(std::ofstream& file, bool first) {
        //
        // We want to display alternatively one node from the right (No) branch,
        // and one from the left (Yes) branch
        // We must print the rightmost non-already-printed Animal of each branch, so
        // we destroy each node once it's been printed.
        //
        if (no_.get()) {
            if (!first) {
                file << "No" << std::endl;
            }
            KnowledgeItem::Ptr* toReset = &no_;
            file << no_->getRightMostAnimal(toReset)->getText() << std::endl;
            toReset->reset();
        }
        file << "No" << std::endl;
        if (yes_.get()) {
            KnowledgeItem::Ptr* toReset = &yes_;
            file << yes_->getRightMostAnimal(toReset)->getText() << std::endl;
            toReset->reset();
        }
        file << getText() << std::endl;
        if (no_.get()) {
            file << "No" << std::endl;
            no_->display_tree(file, false);
        }
        if (yes_.get()) {
            file << "Yes" << std::endl;
            yes_->display_tree(file, false);
        }
    }
    virtual const KnowledgeItem* getRightMostAnimal(KnowledgeItem::Ptr*& toReset) {
        // toReset is set to the value of the shared_ptr to reset in order to destroy the node returned
        toReset = &no_;
        return no_->getRightMostAnimal(toReset);
    }
protected:
    virtual std::string get_question_to_ask(Messenger::Ptr messenger) const {
        return getText() + " ";
    }

public: // TODO
    Ptr yes_;
    Ptr no_;
};


void Animal::toNoNode(Messenger::Ptr messenger, const Ptr& root, Ptr*& previous, Ptr& current) {
    std::string newAnimalName = messenger->ask_new_animal_name();
    Ptr newAnimal(new Animal(newAnimalName));

    std::string discriminate_new_animal = messenger->ask_yes_no_question(newAnimalName, current->getText());

    Ptr new_root(new Question(discriminate_new_animal, newAnimal, current));
    *previous = new_root;
    current = root;

    messenger->say_play_again();
    messenger->say_think();
}


bool Animal::toYesNode(Messenger::Ptr messenger, const Ptr& root, Ptr*& previous, Ptr& current) {
    current = root;
    messenger->say_i_win();
    messenger->say_think();
    return true;
}

void Question::toNoNode(Messenger::Ptr messenger, const Ptr& root, Ptr*& previous, Ptr& current) {
    previous = &no_;
    current = no_;
}

bool Question::toYesNode(Messenger::Ptr messenger, const Ptr& root, Ptr*& previous, Ptr& current) {
    previous = &yes_;
    current = yes_;
    return false;
}


void run(Messenger::Ptr messenger, KnowledgeItem::Ptr& root) {
    KnowledgeItem::Ptr* previous(&root);
    KnowledgeItem::Ptr  current(root);

    messenger->say_hello();
    messenger->say_think();  // TODO: it's odd that this only appears once...  
                             // Others are inside toYesNode or toNoNode...
    bool stop = false;
    while (!stop) {
        switch (current->ask_question(messenger)) {
        case KnowledgeItem::Quit:
            messenger->say_goodbye();
            stop = true;
            break;
        case KnowledgeItem::Yes:
            current->toYesNode(messenger, root, previous, current);
            break;
        case KnowledgeItem::No:
            current->toNoNode(messenger, root, previous, current);
            break;
        default:
            assert(0);
        }
    }
}


int main() {

    KnowledgeItem::Ptr root(new Animal("cat"));

    const std::string fileName = "AnimalGame.memory";
    {
        std::ifstream file(fileName);

        class Muted_Messenger : public Cin_Cout_Messenger {
        public:
            Muted_Messenger(std::istream& input = std::cin)
                : Cin_Cout_Messenger(input)
            {}
            virtual void say(std::string s) {}
        };

        Messenger::Ptr memoryLoader(new Muted_Messenger(file));
        if (!file.bad()) {
            memoryLoader->get_string(); // skip the first 'cat'
        }
        run(memoryLoader, root);
        file.close();   // TODO: be exception-safe
    }

    Messenger::Ptr messenger (new Cin_Cout_Messenger);
    run(messenger, root);

    {
        std::ofstream file(fileName);
        root->display_tree(file, true);
        file.close();  //TODO: should be exception-safe
    }
    return 0;
}