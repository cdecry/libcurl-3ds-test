#ifndef HTML_PARSER_H
#define HTML_PARSER_H

#include "../utility/common.h"
#include "../utility/output.h"
#include <unordered_map>
#include <unordered_set>

struct HTMLElement {
    std::string tag;
    std::unordered_map<std::string, std::string> attributes;
    std::unordered_map<std::string, std::string> styles;
    std::vector<HTMLElement*> children;

    ~HTMLElement() {
        for (auto child : children) {
            delete child;
        }
    }
};

class HTMLParser {
public:
    std::stringstream stream;
    HTMLParser(const std::string& html);
    HTMLElement* parse();
    void renderHTMLTree(Output* output, HTMLElement* root, int depth = 0);

private:
    std::string html;
    size_t index;

    void parseElement(HTMLElement* parent);
    void parseTag(HTMLElement* element);
    std::string extractTagName();
    std::unordered_map<std::string, std::string> extractAttributes();
    bool isSelfClosingTag(const std::string& tag) const;
};

#endif