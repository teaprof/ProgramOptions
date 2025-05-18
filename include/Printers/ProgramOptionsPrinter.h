#ifndef __PROGRAM_OPTIONS_PRINTER_H__
#define __PROGRAM_OPTIONS_PRINTER_H__

#include <ProgramOptionsParser.h>
#include <Printers/PrettyPrinter.h>

class ProgramOptionsPrinter {
    public:
    std::shared_ptr<AbstractSection> print(ProgramOptionsParser& parser) {
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
    std::shared_ptr<AbstractSection> print(OptionsGroup& grp) const {
        auto res = std::make_shared<TextSection>();
        res->setTitle(grp.groupName());
        res->setHeader(grp.description.view());
        res->body_<<grp.detailedList().view();
        return res;
    }    
    std::set<std::string> options_groups_printed_already_;
};
#endif __PROGRAM_OPTIONS_PRINTER_H__