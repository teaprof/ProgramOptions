#ifndef __ABSTRACT_TEXT_SECTION__
#define __ABSTRACT_TEXT_SECTION__

#include <sstream>
#include <vector>
#include <memory>

class TextSectionBase {
    public:
    virtual std::stringstream title() const = 0;
    virtual std::stringstream header() const = 0;
    virtual std::stringstream body() const = 0;
    virtual std::stringstream footer() const = 0;
    virtual std::vector<std::shared_ptr<TextSectionBase>> subsections() const = 0;
};

#endif // __ABSTRACT_TEXT_SECTION__