#ifndef __PROGRAM_SUBCOMMANDS_PRINTER_H__
#define __PROGRAM_SUBCOMMANDS_PRINTER_H__

#include <SubcommandsParser.h>
#include <Printers/PrettyPrinter.h>

class ProgramSubcommandsPrinter {
    public:
    std::shared_ptr<AbstractSection> print(SubcommandsParser& parser) {
        auto res = std::make_shared<TextSection>();
        auto usage = std::make_shared<TextSection>();
        usage->title_<<"Usage:\n";
        for(auto& subcmd : parser.subcommands_order_) {
            usage->header_<<"\t"<<shortHelp(parser, subcmd)<<"\n";
        }

        auto description = std::make_shared<TextSection>();
        description->title_<<"Detailed description:";
        description->header_<<parser.program_description;

        auto details = std::make_shared<TextSection>();
        details->title_<<"Details:";
        for(auto& subcmd : parser.subcommands_order_) {
            auto ptr = subcmd->second;
            for(auto &it : print(*ptr)) {
                details->subsections_.push_back(it);
            }
        }

        res->subsections_.push_back(usage);
        res->subsections_.push_back(description);
        res->subsections_.push_back(details);
        return res;
    }
    std::string shortHelp(SubcommandsParser& parser, SubcommandsParser::subcommands_t::iterator it) const {
        std::stringstream str;
        str<<parser.exename<<" ";
        if(it->first == parser.default_subcommand_name_) {
            if(parser.show_default_subcommand_name_) {
                str<<"["<<it->first<<"] ";
            }
        } else {
            str<<it->first<<" ";
        }
        const std::shared_ptr<ProgramOptionsParser> opts = it->second;
        for(auto group : opts->groups()) {
            str<<"["<<group->groupName()<<"] ";
        }        
        return str.str();
    }
    std::vector<std::shared_ptr<AbstractSection>> print(ProgramOptionsParser& parser) {
        std::vector<std::shared_ptr<AbstractSection>>  res;
        for(auto it : parser.groups()) {
            if(!options_groups_printed_already_.contains(it->groupName())) {
                res.push_back(print(*it));
                options_groups_printed_already_.insert(it->groupName());
            }
        }
        return res;
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

#endif // __PROGRAM_SUBCOMMANDS_PRINTER_H__