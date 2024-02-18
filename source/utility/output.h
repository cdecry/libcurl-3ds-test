#pragma once

#ifndef OUTPUT_HEADER
#	define OUTPUT_HEADER

#include "common.h"

static const int CONSOLE_MAX_ROW = 29;

class Output {
private:
	std::vector<std::string> messageLog;
	bool inReverse;
	
public:
	Output();
	~Output();
    int rowStart = 1;
    void setRowStart(int value);
    int getRowStart();
	void print(std::string message);
	void clear();
	void printAll();
	void setReverseFlag(const bool value);
	bool getReverseFlag() const;
};

#endif
