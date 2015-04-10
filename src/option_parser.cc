#include "option_parser.hh"
#include <iostream>
#include <sstream>
#include <stdarg.h>
#include <algorithm>


using namespace std;
using namespace wt::Opt;

struct str_convert_toupper {
   void operator()(char& c) { c = ::toupper((unsigned char)c); }
};

RuleInterface::RuleInterface(Parser *parser)
  : _parser(parser)
{
  // Self-register the rule to the parser...
  _parser->addRule(this);
}

RuleInterface::~RuleInterface()
{
}


RuleInterface &
RuleInterface::operator |(RuleInterface &other)
{
  RuleInterface *rule = new OneOfRule(this->_parser, 2, this, &other);
  return *(rule);
}


RuleInterface &
RuleInterface::operator ,(RuleInterface &other)
{
  RuleInterface *rule = new ListRule(this->_parser, 2, this, &other);
  _parser->addRule(rule);
  return *(rule);
}


OptionRule::OptionRule(Parser *parser, const string &identifier, const string &long_name, bool is_flag, char short_name)
  : RuleInterface(parser), _short_name(short_name), _id(identifier), _long_name(long_name), _is_flag(is_flag)
{
}

OptionRule::~OptionRule() {
  // Pass...
}

bool
OptionRule::operator()(const char *argv[], int argc, size_t &idx, Parser &parser)
{
  // If there is no option left on stack -> fail
  if(idx == (size_t)argc) {
    return false;
  }
  // copy option
  string str(argv[idx]);
  // if option string is shorter than 2 chars -> fail (this can not be an option)
  if(2 > str.size()) {
    return false;
  }
  // if option string does not start with '-' -> fail
  if('-' != str[0]) {
    return false;
  }
  // handle short option
  if(_short_name == str[1]) {
    if(2 != str.size()) {
      return false;
    }
    if(! _is_flag) {
      // consume option:
      idx++;
      // get value: if there is no value
      if(idx == (size_t)argc) {
        return false;
      }
      parser.set_option(_id, argv[idx]); idx++;
    } else {
      parser.set_flag(_id); idx++;
    }
    return true;
  }

  // Handle long option:
  if('-' != str[1]) {
    return false;
  }
  // handle flag;
  if(_is_flag) {
    if(0 != str.substr(2).compare(_long_name)) {
      return false;
    }
    // consume option:
    parser.set_flag(_id); idx++;
    return true;
  }

  // handle value option
  size_t name_end;
  // If VALUE is missing:
  if(str.npos == (name_end = str.find("="))) {
    return false;
  }
  // Check option name
  if(str.substr(2, name_end-2) != this->_long_name) {
    return false;
  }
  // Consume option...
  parser.set_option(this->_id, str.substr(name_end+1)); idx++;
  return true;
}

string
OptionRule::dump()
{
  stringstream out;
  if(0 != _short_name) {
    out << "(--" << _long_name << "|-" << _short_name << ")";
  } else {
    out << "--" << _long_name;
  }

  if(!_is_flag) {
    out << "=VALUE";
  }
  return out.str();
}


KeywordRule::KeywordRule(Parser *parser, const string &keyword)
  : RuleInterface(parser), _keyword(keyword)
{
  // pass...
}

KeywordRule::~KeywordRule() {
  // pass...
}

bool
KeywordRule::operator ()(const char *argv[], int argc, size_t &idx, Parser &prs) {
  // If there is no option left on stack -> fail
  if(idx == (size_t)argc) { return false; }
  // get string
  std::string str(argv[idx]);
  // compare
  if (_keyword != str) { return false; }
  // register keyword with parser
  prs.add_keyword(_keyword); idx++;
  return true;
}

std::string
KeywordRule::dump() {
  return _keyword;
}


OneOfRule::OneOfRule(Parser *parser)
  : RuleInterface(parser)
{
}

OneOfRule::OneOfRule(Parser *parser, std::list<RuleInterface *> &opts)
  : RuleInterface(parser), options(opts)
{
}


OneOfRule::OneOfRule(Parser *parser, size_t n, ...)
  : RuleInterface(parser)
{
  va_list args; va_start(args, n);
  for (size_t i=0; i<n; i++)
  {
    this->options.push_back(va_arg(args, RuleInterface *));
  }
  va_end(args);
}


OneOfRule::~OneOfRule()
{
}


bool
OneOfRule::operator()(const char *argv[], int argc, size_t &idx, Parser &parser)
{
  list<RuleInterface *>::iterator it = this->options.begin();
  for(; it != this->options.end(); it++) {
    size_t tmp_idx = idx;
    if((**it)(argv, argc, tmp_idx, parser)) {
      idx = tmp_idx;
      return true;
    }
  }

  return false;
}

std::string
OneOfRule::dump()
{
  stringstream out;
  list<RuleInterface *>::iterator it=this->options.begin();

  out << "(";
  if(0 != this->options.size())
    {
      for(size_t i=0; i<this->options.size()-1; i++)
  {
    out << (*it)->dump() << "|";
    it++;
  }
      out << (*it)->dump();
    }
  out << ")";

  return out.str();
}


ValueRule::ValueRule(Parser *parser, const string &identifier)
  : RuleInterface(parser), _id(identifier)
{
  // Pass...
}


ValueRule::~ValueRule() {
  // pass...
}

bool
ValueRule::operator()(const char *argv[], int argc, size_t &idx, Parser &parser)
{
  if(idx == (size_t)argc) {
    return false;
  }
  // consume option
  parser.add_value(_id, argv[idx]); idx++;
  return true;
}

std::string
ValueRule::dump()
{
  if (_id.empty()) {
    return "VALUE";
  } else {
    std::string name = _id;
    std::transform(_id.begin(), _id.end(), name.begin(), ::toupper);
    return name;
  }
}


ListRule::ListRule(Parser *parser, std::list<RuleInterface *> &opts)
  : RuleInterface(parser), options(opts)
{
}


ListRule::ListRule(Parser *parser, size_t n, ...)
  : RuleInterface(parser), options()
{
  va_list args; va_start(args, n);
  for (size_t i=0; i<n; i++)
  {
    this->options.push_back(va_arg(args, RuleInterface *));
  }
  va_end(args);
}


ListRule::~ListRule() {
  // Pass...
}

bool
ListRule::operator()(const char *argv[], int argc, size_t &idx, Parser &parser)
{
  if(idx == (size_t)argc) { return false; }

  size_t tmp_idx = idx;
  list<RuleInterface *>::iterator it=this->options.begin();
  for(; it != this->options.end(); it++) {
    if(! (**it)(argv, argc, tmp_idx, parser)) {
      return false;
    }
  }
  idx = tmp_idx;
  return true;
}

std::string
ListRule::dump()
{
  stringstream out;
  list<RuleInterface *>::iterator it=this->options.begin();

  out << "(";
  if(0 != this->options.size())
    {
      for(size_t i=0; i<this->options.size()-1; i++)
  {
    out << (*it)->dump() << " ";
    it++;
  }
      out << (*it)->dump();
    }
  out << ")";

  return out.str();
}


OptionalRule::OptionalRule(Parser *parser, RuleInterface *rule)
  : RuleInterface(parser), rule(rule)
{
  // Pass...
}

OptionalRule::~OptionalRule()
{
  // pass...
}

bool
OptionalRule::operator()(const char *argv[], int argc, size_t &idx, Parser &parser)
{
  if(idx == (size_t)argc)
    return true;

  size_t tmp_idx = idx;
  if((*(this->rule))(argv, argc, tmp_idx, parser)) {
    idx = tmp_idx;
  }

  return true;
}

std::string
OptionalRule::dump()
{
  stringstream out;

  out << this->rule->dump() << "?";

  return out.str();
}


/* ******************************************************************************************* *
 * Implementation of Option Parser:
 * ******************************************************************************************* */
Parser::Parser()
  : _rule(0)
{
  // Pass...
}

Parser::~Parser()
{
  // Free all associated rules.
  for (std::set<RuleInterface *>::iterator item = _rules.begin(); item!=_rules.end(); item++) {
    delete *item;
  }
}


void
Parser::setGrammar(RuleInterface &rule) {
  _rule = &rule;
}

void
Parser::addRule(RuleInterface *rule) {
  _rules.insert(rule);
}


bool
Parser::parse(const char *argv[], int argc)
{
  size_t idx = 1;

  if(! (*(this->_rule))(argv, argc, idx, *this)) {
    return false;
  }

  if(idx != (size_t)argc) {
    return false;
  }

  return true;
}

std::string
Parser::format_help(std::string prg_name) {
  stringstream out;
  out << "Usage: " << prg_name << " " << _rule->dump() << endl;

  return out.str();
}

void
Parser::set_flag(const string &name) {
  _present_flags.insert(name);
}

bool
Parser::has_flag(const std::string &name) {
  return (0 != this->_present_flags.count(name));
}


void
Parser::set_option(const std::string &name, const string &value) {
  if (0 == this->_present_options.count(name)) {
    this->_present_options[name] = std::list<std::string>();
  }
  this->_present_options[name].push_back(value);
}

bool
Parser::has_option(std::string name) {
  return 0 != this->_present_options.count(name);
}

const std::list<std::string> &
Parser::get_option(const std::string &name) {
  return this->_present_options[name];
}

const std::list<string> &
Parser::get_values(const std::string &identifier) {
  return _present_values[identifier];
}

bool
Parser::has_values(const std::string &identifier) const {
  return (0 != _present_values.count(identifier));
}

void
Parser::add_value(const string &identifier, const string &value) {
  if (0 == _present_values.count(identifier)) {
    _present_values[identifier] = std::list<std::string>();
  }
  _present_values[identifier].push_back(value);
}

bool
Parser::has_keyword(const string &keyword) const {
  return (0 != _present_keywords.count(keyword));
}

void
Parser::add_keyword(const string &keyword) {
  _present_keywords.insert(keyword);
}

ZeroOrMoreRule::ZeroOrMoreRule(Parser *parser, RuleInterface *rule)
: RuleInterface(parser), rule(rule)
{
}

ZeroOrMoreRule::~ZeroOrMoreRule()
{
}

bool
ZeroOrMoreRule::operator()(const char *argv[], int argc, size_t &idx, Parser &parser)
{
  if(idx == (size_t)argc)
    return true;

  size_t tmp_idx = idx;
  while ((*(this->rule))(argv, argc, tmp_idx, parser)) {
    idx = tmp_idx;
  }

  return true;
}


std::string
ZeroOrMoreRule::dump()
{
  stringstream out;

  out << this->rule->dump() << "*";

  return out.str();
}
