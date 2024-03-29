#pragma once

#ifndef OUTPUT_HEADER
#	define OUTPUT_HEADER

#include "common.h"
#include <chrono>
#include <thread>

static const int CONSOLE_MAX_ROW = 29;

class Output {
private:
	std::vector<std::string> messageLog;
	bool inReverse;
    int rowStart = 1;
	
public:
	Output();
	~Output();
	void print(std::string message);
	void clear();
	void printAll();
	void setReverseFlag(const bool value);
	bool getReverseFlag() const;
    void setRowStart(int value);
    int getRowStart();
    void scroll_down();
    void scroll_up();
};

#endif
