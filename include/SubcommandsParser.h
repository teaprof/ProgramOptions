#ifndef __SUBCOMMANDS_PARSER_H__
#define __SUBCOMMANDS_PARSER_H__

#include <AbstractOptionsParser.h>
#include <OptionsGroup.h>
#include <Printers/PrettyPrinter.h>
#include <ProgramOptionsParser.h>

class SubcommandsParser : public AbstractOptionsParser {
    using value_t = std::shared_ptr<ProgramOptionsParser>;
    using subcommands_t = std::map<std::string, value_t>;
    public:
    SubcommandsParser(const std::string& exename) : AbstractOptionsParser(exename) {}
    SubcommandsParser(int argc, char* argv[]) : AbstractOptionsParser(argc, argv) {}
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
            pos = subcommands_.emplace(subcommand_name, std::make_shared<ProgramOptionsParser>(exename)).first;
            subcommands_order_.push_back(subcommands_.find(subcommand_name));
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

    bool activated{false}; //becomes true when parse function succeeded
private:
    subcommands_t subcommands_;    
    std::vector<subcommands_t::iterator> subcommands_order_; //order of subcommands_ for printing purpose
    subcommands_t::iterator selected_subcommand_;
    std::string default_subcommand_name_{"default"};
    bool show_default_subcommand_name_{true};
    friend class ProgramSubcommandsPrinter;
};



#endif // __SUBCOMMANDS_PARSER_H__