#include "htmlParser.h"

HTMLParser::HTMLParser(const std::string& html) : html(html), index(0) {}

HTMLElement* HTMLParser::parse() {
    HTMLElement* root = new HTMLElement;
    parseElement(root);
    return root;
}

// <body><p>hi</p></body>
void HTMLParser::parseElement(HTMLElement* parent) {
    while (index < html.size()) {
        if (html[index] == '<') {
            if (html[index + 1] == '!') {
                // doctype or comment
                index += 2;
                if (html.substr(index, 8) == "DOCTYPE" || html.substr(index, 8) == "doctype") {
                    while (index < html.size() && html[index] != '>') index++;
                    index++;
                }
            } else if (html[index + 1] == '/') {
                // closing tag
                index += 2;
                extractTagName();
                return;
            } else {
                // opening tag
                index++;
                HTMLElement* child = new HTMLElement;
                parseTag(child);
                parent->children.push_back(child);

                if (isSelfClosingTag(child->tag))
                    continue;
                parseElement(child);
            }
        } else {
            index++;
        }
    }
}

void HTMLParser::parseTag(HTMLElement* element) {
    element->tag = extractTagName();
    element->attributes = extractAttributes();
    // Skip to the end of the tag
    while (index < html.size() && html[index] != '>') index++;
    index++;
}

std::string HTMLParser::extractTagName() {
    std::string tagName;
    while (index < html.size() && !isspace(html[index]) && html[index] != '/' && html[index] != '>') {
        tagName += html[index];
        index++;
    }
    return tagName;
}

// <meta />

std::unordered_map<std::string, std::string> HTMLParser::extractAttributes() {
    std::unordered_map<std::string, std::string> attributes;
    while (index < html.size() && html[index] != '>' && html[index] != '/') {
        std::string attrName;
        std::string attrValue;
        while (index < html.size() && isspace(html[index])) index++;

        if (html[index] == '/')
            break;

        while (index < html.size() && html[index] != '=' && !isspace(html[index]) && html[index] != '>') {
            attrName += html[index];
            index++;
        }
        while (index < html.size() && html[index] != '=') index++;
        while (index < html.size() && (html[index] == '=' || isspace(html[index]))) index++;
        char quote = html[index];
        index++;
        while (index < html.size() && html[index] != quote) {
            attrValue += html[index];
            index++;
        }
        attributes[attrName] = attrValue;
        index++;

        while (index < html.size() && isspace(html[index])) index++;
    }
    if (html[index] == '/')
        index ++;
    return attributes;
}

bool HTMLParser::isSelfClosingTag(const std::string& tag) const {
    // List of self-closing tags
    static const std::unordered_set<std::string> selfClosingTags = {
        "area", "base", "br", "col", "embed", "hr", "img", "input",
        "link", "meta", "param", "source", "track", "wbr"
    };
    return selfClosingTags.count(tag) > 0;
}

void HTMLParser::renderHTMLTree(Output* output, HTMLElement* root, int depth) {
    if (!root) return;

    // Print indentation
    for (int i = 0; i < depth; ++i) {
        this->stream << "  ";
    }

    // Print element tag
    this->stream << "<" << root->tag;

    // Print attributes
    for (const auto& attr : root->attributes) {
        this->stream << " " << attr.first << "=\"" << attr.second << "\"";
    }
    this->stream << ">" << "\n";

    // Render children
    for (auto child : root->children) {
        renderHTMLTree(output, child, depth + 1);
    }

    // this->stream << root->text;

    // Print closing tag
    for (int i = 0; i < depth; ++i) {
        this->stream << "  ";
    }
    this->stream << "</" << root->tag << ">" << "\n";
}