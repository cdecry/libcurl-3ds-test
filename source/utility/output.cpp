#include "output.h"

void Output::setRowStart(int value) {
    this->rowStart = value;
}

int Output::getRowStart() {
    return this->rowStart;
}

std::vector<std::string> splitMessageIntoLines(const std::string& message) {
    std::vector<std::string> lines;
    std::istringstream iss(message);
    std::string line;
    while (std::getline(iss, line)) {
        // If the line is longer than 40, split it.
        while (line.length() > 40) {
            lines.push_back(line.substr(0, 40));
            line = line.substr(40);
        }
        lines.push_back(line);
    }
    return lines;
}

Output::Output() : inReverse(false) {
	this->clear();
}

Output::~Output() {
	this->clear();
}

void Output::print(std::string message){
	this->messageLog.push_back(message);
	if (this->messageLog.size() > CONSOLE_MAX_ROW){
		this->messageLog.erase(this->messageLog.begin());
	}
	this->printAll();
}

void Output::clear() {
	while (!this->messageLog.empty()){
		this->messageLog.erase(this->messageLog.begin());
	}
}

void Output::printAll(){
	consoleClear();
    int currentRow = rowStart;
	if (this->inReverse){
        for (int i = this->messageLog.size() - 1; i >= 0; --i) {
            // Split the message into multiple lines
            std::vector<std::string> lines = splitMessageIntoLines(this->messageLog[i]);
            
            // Print each line of the message
            for (const auto& line : lines) {
                text(currentRow, 0, line);
                ++currentRow;
                if (currentRow >= CONSOLE_MAX_ROW) // Check if reached the top row
                    return;
            }
        }
	}
	else {
        for (int i = 0; i < (int)this->messageLog.size(); ++i) {
            std::vector<std::string> lines = splitMessageIntoLines(this->messageLog[i]);
            for (int j = 0; j < (int)lines.size(); ++j) {
                if (currentRow + 1 >= CONSOLE_MAX_ROW) // Check if reached the maximum row
                    return;
                if (currentRow > 0) {
                    text(currentRow, 1, lines[j]);
                }
                ++currentRow;
            }
        }
	}
}

void Output::setReverseFlag(const bool value){
	this->inReverse = value;
}

bool Output::getReverseFlag() const {
	return this->inReverse;
}

void Output::scroll_down() {
    this->setRowStart(this->getRowStart()-1);
    this->printAll();
}

void Output::scroll_up() {
    this->setRowStart(this->getRowStart()+1);
    this->printAll();
}