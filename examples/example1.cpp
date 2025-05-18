#include<ProgramOptions.h>

int main(int argc, char* argv[]) {
    ProgramOptionsParser parser(argc, argv);
    auto help_options = std::make_shared<HelpOptions>();
    auto multithreading_options = std::make_shared<MultithreadOptions>();
    parser.addGroup(help_options);
    parser.addGroup(multithreading_options);

    ProgramOptionsPrinter printer;
    auto dom = printer.print(parser);

    PrettyPrinter pp;
    dom->accept(pp);
    return 0;
}
