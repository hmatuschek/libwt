#include <iostream>
#include "wtcli_gen.hh"
#include "wtcli_trafo.hh"
#include "option_parser.hh"

using namespace wt;

int main(int argc, char *argv[])
{
  Opt::Parser parser;

  Opt::RuleInterface &cmd_help =
      parser.Keyword("help");
  Opt::RuleInterface &cmd_version
      = parser.Keyword("version");
  Opt::RuleInterface &cmd_gen =
      ( parser.Keyword("gen"),
        parser.Value("samples"),
        parser.zeroOrMore(
          (parser.Option("delta") | parser.Option("sin"))),
        parser.Value("outfile") );
  Opt::RuleInterface &cmd_trafo =
      ( parser.Keyword("trafo"),
        parser.Option("scales"),
        ( parser.opt(parser.Option("column")),
          parser.Value("infile") ),
        ( parser.opt(
            parser.Flag("abs") |
            parser.Flag("angle") |
            parser.Flag("real") |
            parser.Flag("imag") ),
          parser.Value("outfile") ) );
  parser.setGrammar(
        (cmd_help | cmd_version | cmd_gen | cmd_trafo) );

  if (!parser.parse((const char **)argv, argc)) {
    std::cerr << "Error while parsing arguments." << std::endl
              << parser.format_help("wtcli") << std::endl;
    return -1;
  }

  if (parser.has_keyword("help")) {
    std::cout << "TODO: implement!" << std::endl;
    return 0;
  } else if (parser.has_keyword("version")) {
    std::cout << "TODO: implement!" << std::endl;
    return 0;
  } else if (parser.has_keyword("gen")) {
    return ::wtcli_generate_signal(parser);
  } else if (parser.has_keyword("trafo")) {
    return ::wtcli_transform(parser);
  }

  return -1;
}
