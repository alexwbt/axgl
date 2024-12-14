
#include <args.hxx>
#include <spdlog/spdlog.h>

int main(int argc, char** argv)
{
  args::ArgumentParser parser("Bundles all files in a directory into a binary file.");
  args::HelpFlag help(parser, "help", "Display the help menu.", { 'h', "help" });

  args::Positional<std::string> source(parser, "source", "The source directory to bundle.", args::Options::Required);
  args::Positional<std::string> target(parser, "target", "The output binary file.", args::Options::Required);

  try { parser.ParseCLI(argc, argv); }
  catch (const args::Completion& e) { std::cout << e.what(); }
  catch (const args::Help&) { std::cout << parser; }
  catch (const args::Error& e)
  {
    SPDLOG_ERROR(e.what());
    return 1;
  }
}
