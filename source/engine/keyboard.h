#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <3ds.h>
#include <string>
#include <vector>

class Keyboard {
public:
    Keyboard();
    ~Keyboard();

    // Initialize the keyboard with a specific type and options
    void init(SwkbdType type, int maxChars, int maxLines = 1);

    // Set hint text for the keyboard
    void setHintText(const std::string& hintText);

    // Set callback function for filtering input
    void setFilterCallback(SwkbdCallbackFn callback, void* user = nullptr);

    // Set up special buttons
    void setButtons(const std::string& left, const std::string& middle, const std::string& right);

    // Set other features like predictive input, multiline, etc.
    void setFeatures(u32 features);

    // Handle the input and get the result
    SwkbdButton getInput(std::string& output);

private:
    SwkbdState swkbd;
    // SwkbdStatusData swkbdStatus;
    // SwkbdLearningData swkbdLearning;
    bool reload;

    // Helper to replace certain words with predefined alternatives
    void setDictionary(const std::vector<std::pair<std::string, std::string>>& dictionary);
};

#endif // KEYBOARD_H
