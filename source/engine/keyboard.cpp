#include "keyboard.h"
#include <cstring>

Keyboard::Keyboard() : reload(false) {
    // memset(&swkbdStatus, 0, sizeof(swkbdStatus));
    // memset(&swkbdLearning, 0, sizeof(swkbdLearning));
}

Keyboard::~Keyboard() {
    // Any necessary cleanup
}

void Keyboard::init(SwkbdType type, int maxChars, int maxLines) {
    swkbdInit(&swkbd, type, maxLines, -1);
    swkbdSetValidation(&swkbd, SWKBD_NOTEMPTY_NOTBLANK, 0, 0);
    swkbdSetFeatures(&swkbd, SWKBD_PREDICTIVE_INPUT);
    // swkbdSetStatusData(&swkbd, &swkbdStatus, reload, true);
    // swkbdSetLearningData(&swkbd, &swkbdLearning, reload, true);
    reload = true;
}

void Keyboard::setHintText(const std::string& hintText) {
    swkbdSetHintText(&swkbd, hintText.c_str());
}

void Keyboard::setFilterCallback(SwkbdCallbackFn callback, void* user) {
    swkbdSetFilterCallback(&swkbd, callback, user);
}

void Keyboard::setButtons(const std::string& left, const std::string& middle, const std::string& right) {
    swkbdSetButton(&swkbd, SWKBD_BUTTON_LEFT, left.c_str(), false);
    swkbdSetButton(&swkbd, SWKBD_BUTTON_MIDDLE, middle.c_str(), true);
    swkbdSetButton(&swkbd, SWKBD_BUTTON_RIGHT, right.c_str(), true);
}

void Keyboard::setFeatures(u32 features) {
    swkbdSetFeatures(&swkbd, features);
}

SwkbdButton Keyboard::getInput(std::string& output) {
    char buffer[1024];
    SwkbdButton button = swkbdInputText(&swkbd, buffer, sizeof(buffer));
    output = std::string(buffer);
    return button;
}

void Keyboard::setDictionary(const std::vector<std::pair<std::string, std::string>>& dictionary) {
    std::vector<SwkbdDictWord> words(dictionary.size());
    for (size_t i = 0; i < dictionary.size(); ++i) {
        swkbdSetDictWord(&words[i], dictionary[i].first.c_str(), dictionary[i].second.c_str());
    }
    swkbdSetDictionary(&swkbd, words.data(), words.size());
}
