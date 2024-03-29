//////////////////////////////////////////////////////////////////////////
//
//  Animal Game, 2012, by Xavier Nodet <xavier.nodet@gmail.com>
//
//////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <assert.h>
#include <string>
#include <memory>



//////////////////////////////////////////////////////////////////////////
//
// Input / Output of the game
//
//////////////////////////////////////////////////////////////////////////


//
// We'll have to ask questions and get answers
// Here's an interface for this.
//
class Messenger {
public:
    typedef std::shared_ptr<Messenger> Ptr;

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
    virtual const char* no() = 0;
    virtual const char* quit() = 0;

    virtual void say(std::string s) = 0;
};

//
// A Messenger that uses the console and speaks English
//
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
        say_think();
    }
    void say_goodbye()  {
        const char* goodbye = "Ok. Bye!\n";
        say(goodbye);
    }
    void say_think()  {
        const char* think = "Think of an animal.\n";
        say(think);
    }
    void say_play_again() {
        const char* play_again = "Thanks! Let's play again.\n";
        say(play_again);
    }
    void say_i_win() {
        const char* win = "Ha! I win! Let's play again\n";
        say(win);
    }
    std::string guess_animal(const std::string& animalName) {
        // TODO: use boost::format so that the string can be externalized in one piece
        return "Is your animal a " + animalName + "? ";
    }
    std::string ask_yes_no_question(std::string newAnimalName, std::string badName) {
        // TODO: use boost::format so that the string can be externalized in one piece
        say("What is a yes/no question to tell a " + newAnimalName + " from a " + badName + "? ");
        return get_string();
    }
    std::string ask_new_animal_name() {
        const char* what_is_it = "Oh no. What was it? ";
        say(what_is_it);
        return get_string();
    }

    const char* yes() {return "Yes";}
    const char* no() {return "No";}
    const char* quit() {return "Quit";}

    void say(std::string s) {
        std::cout << s;
    }
private:
    std::istream& input_;
};



//////////////////////////////////////////////////////////////////////////
//
// Data structures to hold the knowledge of the game
//
//////////////////////////////////////////////////////////////////////////

//
// All the knowledge of the game is organized as a binary tree of
// animals (the leaf nodes) and questions that discriminate animals
// (the non-leaf nodes)
//
// This is the root class for both Animal and Question
//
class KnowledgeItem {
public:
    typedef std::shared_ptr<KnowledgeItem> Ptr;

    // All KnowledgeItems know a question to which the answer is Yes or No
    // or Quit, if you're bored...
    enum PossibleAnswers {
        Yes, No, Quit
    };
    // Could be a question that discriminates animals,
    // or checking if user thought about this particular animal
    virtual PossibleAnswers ask_question(Messenger::Ptr messenger) const = 0;

    // Answer to the latest question was 'Yes'
	virtual void toYesNode(Messenger::Ptr messenger, const Ptr& root, Ptr*& previous, Ptr& current) = 0;
    // Answer to the latest question was 'No': we may have to create a new animal
    virtual void toNoNode(Messenger::Ptr messenger, const Ptr& root, Ptr*& previous, Ptr& current) = 0;

    virtual std::string getText() const = 0;

    // A destructive tree-printing method to save the memory of the game
    virtual void display_tree(Messenger::Ptr messenger, std::ofstream& file, bool first) = 0;
    // Returns the leaf that is furthest on the right (the 'No' branches)
    // And toReset is the shared_ptr pointing to this leaf (to reset it)
    virtual const KnowledgeItem* getRightMostAnimal(KnowledgeItem::Ptr*& toReset) = 0;
};


//////////////////////////////////////////////////////////////////////////
//
// The leaf nodes of the tree: an animal
//
//////////////////////////////////////////////////////////////////////////

class Animal : public KnowledgeItem {
public:
    Animal(const std::string text)
        : text_(text)
    {}
    virtual PossibleAnswers ask_question(Messenger::Ptr messenger) const;
    virtual void toYesNode(Messenger::Ptr messenger, const Ptr& root, Ptr*& previous, Ptr& current);
    virtual void toNoNode(Messenger::Ptr messenger, const Ptr& root, Ptr*& previous, Ptr& current);
    
    virtual void display_tree(Messenger::Ptr, std::ofstream&, bool);
    virtual const KnowledgeItem* getRightMostAnimal(KnowledgeItem::Ptr*& toReset);

    virtual std::string getText() const {return text_;}

protected:
    // Converts the text of the node into an appropriate question
    virtual std::string get_question_to_ask(Messenger::Ptr messenger) const;

private:
    // Wait for answer from user and convert it into one of the enum values
    static PossibleAnswers get_answer(Messenger::Ptr messenger);

private:
    std::string text_;
};


//////////////////////////////////////////////////////////////////////////
//
// The inside nodes of the tree: questions to discriminate the animals
//
//////////////////////////////////////////////////////////////////////////

class Question : public Animal {
public:
    Question(const std::string text, Ptr yes, Ptr no)
        : Animal(text)
        , yes_(yes)
        , no_(no)
    {}
    virtual void toYesNode(Messenger::Ptr messenger, const Ptr& root, Ptr*& previous, Ptr& current);
    virtual void toNoNode(Messenger::Ptr messenger, const Ptr& root, Ptr*& previous, Ptr& current);

    virtual void display_tree(Messenger::Ptr messenger, std::ofstream& file, bool first);
    virtual const KnowledgeItem* getRightMostAnimal(KnowledgeItem::Ptr*& toReset);

protected:
    virtual std::string get_question_to_ask(Messenger::Ptr messenger) const {
        return getText() + " ";
    }

private:
    // If it exists, display a node and its sub-tree
    void display_branch(Messenger::Ptr messenger, std::ofstream& file, KnowledgeItem::Ptr node, const std::string s);
    // Find and display the rightmost node, and allow for its deletion by returning the pointing shared_ptr
    void display_node(std::ofstream& file, KnowledgeItem::Ptr* toReset);

private:
    Ptr yes_;
    Ptr no_;
};



//////////////////////////////////////////////////////////////////////////
//
// Methods for Animal
//
// Would be in separate files in a real project...
//
//////////////////////////////////////////////////////////////////////////

KnowledgeItem::PossibleAnswers Animal::ask_question(Messenger::Ptr messenger) const {
    messenger->say(get_question_to_ask(messenger));
    return get_answer(messenger);
}

std::string Animal::get_question_to_ask(Messenger::Ptr messenger) const {
    // An animal asks 'Is your animal a ...?'
    return messenger->guess_animal(getText());
}

KnowledgeItem::PossibleAnswers Animal::get_answer(Messenger::Ptr messenger) {
    std::string s = messenger->get_string();
    if ((s.size() == 0) || (s == messenger->quit())) {
        return Quit;
    } else if (s == messenger->yes()) {
        return Yes;
    } else {
        return No;
    }
}

void Animal::toNoNode(Messenger::Ptr messenger, const Ptr& root, Ptr*& previous, Ptr& current) {
    //
    // An animal has not been guess correctly. We have to create a new Animal, and
    // a discrimination Question
    //
    std::string newAnimalName = messenger->ask_new_animal_name();
    Ptr newAnimal(new Animal(newAnimalName));

    std::string discriminate_new_animal = messenger->ask_yes_no_question(newAnimalName, current->getText());

    KnowledgeItem::Ptr new_root(new Question(discriminate_new_animal, newAnimal, current));
    *previous = new_root;
    current = root;

    messenger->say_play_again();
    messenger->say_think();
}


void Animal::toYesNode(Messenger::Ptr messenger, const Ptr& root, Ptr*& previous, Ptr& current) {
    // Program guessed correctly.  Start over.
    current = root;
    messenger->say_i_win();
    messenger->say_think();
}

void Animal::display_tree(Messenger::Ptr, std::ofstream&, bool) {
    // Intentionally blank...
    // Leaf nodes don't even print themselves: a single 'cat' node
    // is not saved in the memory because it will be re-created in any case
    // before reading
}

const KnowledgeItem* Animal::getRightMostAnimal(KnowledgeItem::Ptr*& toReset) {
    // An animal, being a leaf-node, is the right-most of its tree
    return this;
}


//////////////////////////////////////////////////////////////////////////
//
// Methods for Question
//
// Would be in separate files in a real project...
//
//////////////////////////////////////////////////////////////////////////

void Question::toNoNode(Messenger::Ptr messenger, const Ptr& root, Ptr*& previous, Ptr& current) {
    // Go down the correct branch of the tree
    previous = &no_;
    current = no_;
}

void Question::toYesNode(Messenger::Ptr messenger, const Ptr& root, Ptr*& previous, Ptr& current) {
    // Go down the correct branch of the tree
    previous = &yes_;
    current = yes_;
}

void Question::display_tree(Messenger::Ptr messenger, std::ofstream& file, bool first) {
    //
    // The idea behind memory save/restore is to save a text file that can
    // be read later exactly as if the user will type on the keyboard
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
        display_node(file, &no_);
    }
    file << "No" << std::endl;
    if (yes_.get()) {
        display_node(file, &yes_);
    }
    file << getText() << std::endl;
    display_branch(messenger, file, no_, messenger->no());
    display_branch(messenger, file, yes_, messenger->yes());
}

void Question::display_branch(Messenger::Ptr messenger, std::ofstream& file, KnowledgeItem::Ptr node, const std::string s) {
    if (node.get()) {
        file << s << std::endl;
        node->display_tree(messenger, file, false);
    }
}

void Question::display_node(std::ofstream& file, KnowledgeItem::Ptr* toReset) {
    file << (*toReset)->getRightMostAnimal(toReset)->getText() << std::endl;
    toReset->reset();  // Kill the node, ensuring we print it exactly once
}

const KnowledgeItem* Question::getRightMostAnimal(KnowledgeItem::Ptr*& toReset) {
    // toReset is set to the value of the shared_ptr to reset in order to destroy the node returned
    toReset = &no_;
    return no_->getRightMostAnimal(toReset);
}




//////////////////////////////////////////////////////////////////////////
// 
// Main part of the game
//
//////////////////////////////////////////////////////////////////////////


//
// Start from the root of the tree
// Repeatedly ask a question and take the appropriate branch until the user is fed-up...
// Also used to rebuild the in-game memory
//
void run(Messenger::Ptr messenger, KnowledgeItem::Ptr& root) {
    KnowledgeItem::Ptr* previous(&root);
    KnowledgeItem::Ptr  current(root);

    messenger->say_hello();

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

//
// Read from file the content of the knowledge base saved from a previous session
//
void read_memory(const std::string fileName, KnowledgeItem::Ptr& root)  {
    std::ifstream file(fileName);  // closes automatically

    // Don't print the questions or comments while reading the memory back
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
}

//
// Save to file the content of the knowledge base saved for the next session
//
void save_memory(Messenger::Ptr messenger, const std::string& fileName, KnowledgeItem::Ptr root)  {
    std::ofstream file(fileName);     // closes automatically
    root->display_tree(messenger, file, true);
}

int main(int, char**) {

    // Everybody knows that cats exist, even computers...
    KnowledgeItem::Ptr root(new Animal("cat"));

    const std::string fileName = "AnimalGame.memory";
    read_memory(fileName, root);

    Messenger::Ptr messenger (new Cin_Cout_Messenger);
    run(messenger, root);

    save_memory(messenger, fileName, root);

    return 0;
}