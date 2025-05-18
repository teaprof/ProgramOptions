#ifndef __ABSTRACT_OPTIONS_PARSER_H__
#define __ABSTRACT_OPTIONS_PARSER_H__

#include <boost/program_options.hpp>

class AbstractOptionsParser  {
public:
    virtual bool parse(int argc, const char* argv[]) = 0;
    virtual void validate() = 0;
    virtual void update(const boost::program_options::variables_map& vm) = 0;
    
    std::string program_description;
};


#endif // __ABSTRACT_OPTIONS_PARSER_H__