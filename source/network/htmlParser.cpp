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
                while (index < html.size() && html[index] != '>') index++;
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
            // skip style tag content (needs to be parsed into style obj)
            // skip script tag content
            if (parent->tag == "style") {
                index++;
                continue;
            }
            else if (parent->tag == "script") {
                size_t found = html.find("</script>", index);
                if (found!=std::string::npos) {
                    index = found;
                    continue;
                }
            }

            std::string innerText = "";
            while (index < html.size() && html[index] != '<') {
                innerText += html[index];
                index++;
            }

            std::size_t found = innerText.find_last_not_of(" \t\f\v\n\r>");
            if (found!=std::string::npos)
                innerText.erase(found+1);
            else
                innerText.clear();

            if (!innerText.empty())
                parent->text += innerText;
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
        
        char quote = ' ';
        if (html[index] == '\"' || html[index] == '\'') {
            quote = html[index];
            index++;
        }
        
        // <div id="smoke">
        // <div id=smoke>
        // <div id=smoke >
        while (index < html.size() && html[index] != quote && html[index] != '>') {
            attrValue += html[index];
            index++;
        }
        attributes[attrName] = attrValue;
        
        if (html[index] != '>')
            index++;

        while (index < html.size() && isspace(html[index])) index++;
    }
    if (html[index] == '/')
        index ++;
    return attributes;
}

bool HTMLParser::isWhitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v';
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

    if (!root->text.empty())
        this->stream << "[" << root->text << "]\n";

    // Print closing tag
    for (int i = 0; i < depth; ++i) {
        this->stream << "  ";
    }
    this->stream << "</" << root->tag << ">" << "\n";
}

void HTMLParser::renderTextContent(Output* output, HTMLElement* root, int depth) {
    if (!root) return;

    if (!root->text.empty() && (root->tag == "p" || root->tag == "span" || 
        root->tag == "div" || root->tag == "text" || root->tag == "h1" || 
        root->tag == "h2" || root->tag == "h3" || root->tag == "h4" || 
        root->tag == "h5" || root->tag == "h6" || root->tag == "a")) {

        std::string textWithQuotes = root->text;
        size_t pos = 0;

        while ((pos = textWithQuotes.find("&quot;", pos)) != std::string::npos) {
            textWithQuotes.replace(pos, 6, "\"");
            pos += 1;
        }

        this->stream << textWithQuotes << "\n";
    }

    // Render children
    for (auto child : root->children) {
        renderTextContent(output, child, depth + 1);
    }

    // Add new line after end of div. don't allow triple new line.
    if (root->tag == "div" && !this->stream.str().empty()) {
        const std::string& currentStream = this->stream.str();
        if (currentStream.size() < 2 || currentStream.substr(currentStream.size() - 2) != "\n\n") {
            this->stream << "\n";
        }
    }
}

void HTMLParser::traverseHTMLElement(HTMLElement* element) {
    // Print text if present
    if (!element->text.empty()) {
        this->stream << element->text << "\n";
    }

    // Traverse children
    for (auto child : element->children) {
        traverseHTMLElement(child);
    }
}