#ifndef __PROGRAM_SUBCOMMANDS_PRINTER_H__
#define __PROGRAM_SUBCOMMANDS_PRINTER_H__

#include <SubcommandsParser.h>
#include <Printers/PrettyPrinter.h>

class ProgramSubcommandsPrinter {
    public:
    std::shared_ptr<Section> print(SubcommandsParser& parser) {
        auto res = std::make_shared<Section>();
        auto usage = std::make_shared<Section>();
        usage->title = "Usage:";
        for(auto& subcmd : parser.subcommands_order_) {
            usage->add_paragraph("\t" + shortHelp(parser, subcmd));
        }

        auto description = std::make_shared<Section>();
        description->title = "Detailed description:";
        description->add_paragraph(parser.program_description);

        auto details = std::make_shared<Section>();
        details->title = "Details:";
        for(auto& subcmd : parser.subcommands_order_) {
            auto ptr = subcmd->second;
            for(auto &it : print(*ptr)) {
                details->items.push_back(it);
            }
        }

        res->items.push_back(usage);
        res->items.push_back(description);
        res->items.push_back(details);
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
    std::vector<std::shared_ptr<Section>> print(ProgramOptionsParser& parser) {
        std::vector<std::shared_ptr<Section>>  res;
        for(auto it : parser.groups()) {
            if(!options_groups_printed_already_.contains(it->groupName())) {
                res.push_back(print(*it));
                options_groups_printed_already_.insert(it->groupName());
            }
        }
        return res;
    }
    std::shared_ptr<Section> print(OptionsGroup& grp) const {
        auto res = std::make_shared<Section>();
        res->title = grp.groupName();
        res->add_paragraph(grp.description.str());
        res->add_paragraph(grp.detailedList().str());
        return res;
    }    
    std::set<std::string> options_groups_printed_already_;
};

#endif // __PROGRAM_SUBCOMMANDS_PRINTER_H__