#ifndef __SUBCOMMANDS_PARSER_H__
#define __SUBCOMMANDS_PARSER_H__

#include <AbstractOptionsParser.h>
#include <OptionsGroup.h>
#include <PrettyPrinter/PrettyPrinter.h>
#include <ProgramOptionsParser.h>

class SubcommandsParser : public AbstractOptionsParser {
    using value_t = std::shared_ptr<ProgramOptionsParser>;
    using subcommands_t = std::map<std::string, value_t>;
    public:
    SubcommandsParser() : AbstractOptionsParser() {}    
    std::shared_ptr<ProgramOptionsParser> push_back(const std::string& subcommand_name, std::shared_ptr<ProgramOptionsParser> val) {
        auto res = subcommands_.emplace(subcommand_name, val);
        if(!res.second) {
            throw std::runtime_error("The specified subcommand_name is already present in SubcommandsParser");
        }
        subcommands_order_.push_back(subcommands_.find(subcommand_name));
        return res.first->second;
    }
    std::shared_ptr<ProgramOptionsParser> operator[](const std::string& subcommand_name) {
        auto pos = subcommands_.find(subcommand_name);
        if(pos == subcommands_.end()) {
            pos = subcommands_.emplace(subcommand_name, std::make_shared<ProgramOptionsParser>()).first;
        }
        return pos->second;
    }    
    std::shared_ptr<ProgramOptionsParser> defaultSubcommand() {
        return (*this)[default_subcommand_name_];
    }
    void setDefaultSubcommand(const std::string& subcommand_name) {
        default_subcommand_name_ = subcommand_name;
    }
    std::shared_ptr<ProgramOptionsParser> selectedSubcommand() {
        return selected_subcommand_->second;
    }
    const std::string& selectedSubcommandName() {
        return selected_subcommand_->first;
    }
    bool parse(int argc, const char* argv[]) override {
        assert(!subcommands_.empty());
        if(argc >= 2) {
            const char* first_arg = argv[1];
            selected_subcommand_ = subcommands_.find(first_arg);
            if(selected_subcommand_ != subcommands_.end()) {
                argc--;
                argv++;
            } else {
                // Case: subcommand is unknown, select the default subcommand
                selected_subcommand_ = subcommands_.find(default_subcommand_name_);
                assert(selected_subcommand_ != subcommands_.end());
            }
        } else {
            // Case: no options specified, select the default subcommand
            selected_subcommand_ = subcommands_.find(default_subcommand_name_);
            assert(selected_subcommand_ != subcommands_.end());
        };
        selected_subcommand_->second->parse(argc, argv);
        activated = true;
        return true;
    }
    void validate() override {
    }
    void  update(const boost::program_options::variables_map& vm) override {
    }
    void showDefaultSubcommandName(bool flag) {
        show_default_subcommand_name_ = flag;
    }

    std::string exename{"hypercube"}; /// \todo: exename should be initialized from argv[0], move to AbstractOptionsParser
    bool activated{false}; //becomes true when parse function succeeded
private:
    subcommands_t subcommands_;    
    std::vector<subcommands_t::iterator> subcommands_order_; //order of subcommands_ for printing purpose
    subcommands_t::iterator selected_subcommand_;
    std::string default_subcommand_name_{"default"};
    bool show_default_subcommand_name_{true};
    friend class ProgramSubcommandsPrinter;
};

class ProgramSubcommandsPrinter {
    public:
    std::shared_ptr<TextSectionBase> print(SubcommandsParser& pmo) {
        auto res = std::make_shared<TextSection>();
        auto usage = std::make_shared<TextSection>();
        usage->title_<<"Usage:\n";
        for(auto& subcmd : pmo.subcommands_order_) {
            usage->header_<<"\t"<<shortHelp(pmo, subcmd)<<"\n";
        }

        auto description = std::make_shared<TextSection>();
        description->title_<<"Detailed description:";
        description->header_<<pmo.program_description;

        auto details = std::make_shared<TextSection>();
        details->title_<<"Details:";
        for(auto& subcmd : pmo.subcommands_order_) {
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
    std::string shortHelp(SubcommandsParser& pmo, SubcommandsParser::subcommands_t::iterator it) const {
        std::stringstream str;
        str<<pmo.exename<<" ";
        if(it->first == pmo.default_subcommand_name_) {
            if(pmo.show_default_subcommand_name_) {
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
    std::vector<std::shared_ptr<TextSectionBase>> print(ProgramOptionsParser& pmo) {
        std::vector<std::shared_ptr<TextSectionBase>>  res;
        //res->setTitle(pmo.title);
        //res->setHeader(pmo.description);
        for(auto it : pmo.groups()) {
            if(!options_groups_printed_already_.contains(it->groupName())) {
                res.push_back(print(*it));
                options_groups_printed_already_.insert(it->groupName());
            }
        }
        return res;
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

#endif // __SUBCOMMANDS_PARSER_H__