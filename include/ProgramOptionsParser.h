#ifndef __PROGRAM_OPTIONS_PARSER_H__
#define __PROGRAM_OPTIONS_PARSER_H__

#include <AbstractOptionsParser.h>
#include <OptionsGroup.h>
#include <PrettyPrinter/PrettyPrinter.h>

#include <map>
#include <string> 
#include <sstream>
#include <iostream>
#include <variant>
#include <locale>
#include <set>

class ProgramOptionsParser : public AbstractOptionsParser {
    // This class can parse the list of options and print the help message
    // Use this class for simple set of command line options like: 
    // programname --arg1 --arg2 10 -zxc -v 20 input.txt output.txt
    public:
        ProgramOptionsParser() = default;
        virtual void addGroup(std::shared_ptr<OptionsGroup> options) {
            if(options->positional.max_total_count() != 0) {
                for(auto it : groups_) {
                    //only one group of options is allowed to have positional arguments
                    assert(it->positional.max_total_count() == 0);
                }
            }
            groups_.push_back(options);
        }
        bool parse(int argc, const char* argv[]) override {
            namespace po = boost::program_options;
            boost::program_options::options_description partial;
            boost::program_options::positional_options_description positional;        
            for(auto it : groups_) {
                partial.add(it->partial);
                if(it->positional.max_total_count() != 0) {
                    //only one group of options is allowed to have positional arguments
                    assert(positional.max_total_count() == 0);
                    positional = it->positional;
                }
            }
            boost::program_options::variables_map vm;
            po::store(po::command_line_parser(argc, argv).options(partial).positional(positional).run(), vm);
            boost::program_options::notify(vm);
            for(auto it : groups_) {
                it->update(vm);
            }
            activated = true;
            return true;
        }
        void validate() override {
            for(auto it : groups_) 
                it->validate();
        }
        void  update(const boost::program_options::variables_map& vm) override {

        }
        const std::vector<std::shared_ptr<OptionsGroup>> groups() const {
            return groups_;
        }

        std::string title;
        std::string description;
        bool activated{false}; //becomes true when parse function succeeded
    private:
        std::vector<std::shared_ptr<OptionsGroup>> groups_;
};

class ProgramOptionsPrinter {
    public:
    std::shared_ptr<TextSectionBase> print(ProgramOptionsParser& parser) {
        auto res = std::make_shared<TextSection>();
        auto usage = std::make_shared<TextSection>();
        usage->title_<<"Usage:\n";
        usage->header_<<"\t"<<shortHelp(parser)<<"\n";

        auto description = std::make_shared<TextSection>();
        description->title_<<"Detailed description:";
        description->header_<<parser.program_description;

        auto details = std::make_shared<TextSection>();
        details->title_<<"Details:";
        for(auto& group : parser.groups()) {
            details->subsections_.push_back(print(*group));
        }

        res->subsections_.push_back(usage);
        res->subsections_.push_back(description);
        res->subsections_.push_back(details);
        return res;
    }
    std::string shortHelp(ProgramOptionsParser& parser) const {
        std::stringstream str;
        for(auto group : parser.groups()) {
            str<<"["<<group->groupName()<<"] ";
        }        
        return str.str();
    }
    std::shared_ptr<TextSectionBase> print(OptionsGroup& grp) const {
        auto res = std::make_shared<TextSection>();
        res->setTitle(grp.groupName());
        res->setHeader(grp.description.view());
        res->body_<<grp.detailedList().view();
        return res;
    }    
    std::set<std::string> options_groups_printed_already_;
};
#endif // __PROGRAM_OPTIONS_PARSER_H__