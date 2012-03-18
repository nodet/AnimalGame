#include <iostream>
#include <assert.h>
#include <string>
#include <memory>




template <typename Messenger>
class KnowledgeItem {
public:
    typedef std::shared_ptr<KnowledgeItem> Ptr;
    enum PossibleAnswers {
        Yes, No, Quit
    };
    virtual PossibleAnswers ask_question() const = 0;

	virtual bool toYesNode(const Ptr& root, Ptr& current) = 0;
    virtual void toNoNode(Ptr& root, Ptr& current) = 0;

    virtual std::string getText() const = 0;

};


template <typename Messenger>
class Animal : public KnowledgeItem<Messenger> {
public:
    Animal(const std::string text)
        : text_(text)
    {}
    virtual PossibleAnswers ask_question() const {
        std::cout << getQuestion();
        return get_answer();
    }
    virtual bool toYesNode(const Ptr& root, Ptr& current);
    virtual void toNoNode(Ptr& root, Ptr& current);

protected:
    virtual std::string getText() const {return text_;}
    virtual std::string getQuestion() const {
        return "Is your animal a " + getText() + "? ";
    }

private:
    static PossibleAnswers get_answer() {
        std::string s = Messenger::get_string(std::cin);
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


template <typename Messenger>
class Question : public Animal<Messenger> {
public:
    Question(const std::string text, Ptr yes, Ptr no)
        : Animal(text)
        , yes_(yes)
        , no_(no)
    {}
	virtual bool toYesNode(const Ptr& root, Ptr& current);
    virtual void toNoNode(Ptr& root, Ptr& current);

protected:
    virtual std::string getQuestion() const {
        return getText() + " ";
    }

private:
    Ptr yes_;
    Ptr no_;
};


template <typename Messenger>
void Animal<Messenger>::toNoNode(Ptr& root, Ptr& current) {
    const char* what_is_it = "Oh no. What was it? ";
    std::cout << what_is_it ;
    std::string newAnimalName = Messenger::get_string(std::cin);
    Ptr newAnimal(new Animal(newAnimalName));

    std::cout << "What is a yes/no question to tell a " 
        << newAnimalName << " from a " 
        << current->getText() << "? ";
    std::string discriminate_new_animal = Messenger::get_string(std::cin);

    Ptr new_root(new Question<Messenger>(discriminate_new_animal, newAnimal, current));
    root = new_root;
    current = root;

    std::cout << "Thanks! Let's play again." << std::endl;
    Messenger::say_think();
}


template <typename Messenger>
bool Animal<Messenger>::toYesNode(const Ptr& root, Ptr& current) {
    current = root;
    const char* win = "Ha! I win! Let's play again";
    std::cout << win << std::endl;
    Messenger::say_think();
    return true;
}

template <typename Messenger>
void Question<Messenger>::toNoNode(Ptr& root, Ptr& current) {
    current = no_;
}

template <typename Messenger>
bool Question<Messenger>::toYesNode(const Ptr& root, Ptr& current) {
    current = ((Question*)root.get())->yes_;
    return false;
}


template <typename Messenger>
void run() {
    // Specialize the template classes with our Messenger
    typedef KnowledgeItem<Messenger> KnowledgeItem;
    typedef Animal<Messenger>        Animal;

    KnowledgeItem::Ptr root(new Animal("cat"));
    KnowledgeItem::Ptr current(root);

    Messenger::say_hello();
    Messenger::say_think();  // TODO: it's odd that this only appears once...  
                             // Others are inside toYesNode or toNoNode...
    while (1) {
        switch (current->ask_question()) {
        case KnowledgeItem::Quit:
            Messenger::say_goodbye();
            return;
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

}


class Cin_Cout_Messenger {
    // All input/output gathered as static methods of this class
    // intended to be a template parameter of the others.

    // Individual messages could be made static members of some other class
    // to further decouple the language from the fact that we use cin/cout.
public:
    static std::string get_string(std::istream& s) {
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
    static void say_hello()  {
        const char* intro = "Hi, let's play the animal game. You can quit at any time by typing \"Quit.\"\n";
        std::cout << intro << std::endl;
    }
    static void say_goodbye()  {
        const char* goodbye = "Ok. Bye!";
        std::cout << goodbye << std::endl;
    }
    static void say_think()  {
        std::cout << "Think of an animal." << std::endl;
    }
};







int main() {
    run<Cin_Cout_Messenger>();
    return 0;
}