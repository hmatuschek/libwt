#ifndef __WT_OPTION_PARSER_HH__
#define __WT_OPTION_PARSER_HH__

#include <list>
#include <string>
#include <set>
#include <map>


namespace wt{
namespace Opt {


// forward declaration:
class Parser;

/** Interface class for all option rules. */
class RuleInterface
{
public:
  /** Constructor, @c parser specifies the parser. */
  RuleInterface(Parser *parser);

  /** Destructor. */
  virtual ~RuleInterface();

  /** This method will be implemented by all option rules to perform option parsing.
   * @param argv Array of arguments provided from main().
   * @param argc Number of arguments given by main().
   * @param idx Reference to the index of the argument being processed currently.
   * @param parser Reference to the OptionParser instance to store values of options and
   *   arugmuemnts. */
  virtual bool operator()(const char *argv[], int argc, size_t &idx, Parser &parser) = 0;

  /** This mehtod will be implemented to generate a string representation of the option
   * rules. */
  virtual std::string dump() = 0;

  /** Constructs a list of alternatives: @c OneOfRule. */
  RuleInterface &operator| (RuleInterface &other);

  /** Constructs a @c ListRule option-rule. */
  RuleInterface &operator, (RuleInterface &other);

protected:
  /** Holds a weak reference to the option parser instance. */
  Parser *_parser;
};



/** A rule representing an option or flag.
 * @ingroup optionparser
 */
class OptionRule: public RuleInterface
{
protected:
  /** Constructor.
   * @param parser Specifies the parser that owns this rule.
   * @param long_name Specifies the long name for the option.
   * @param is_flag Specifies if the option is a flag and do not take a value.
   * @param short_name Specifies the short name for the option. By default it is 0 this
   *    means there is no short name. */
  OptionRule(Parser *parser, const std::string &identifier, const std::string &_long_name,
             bool _is_flag = false, char _short_name=0);

public:
  /** Destructor. */
  virtual ~OptionRule();

  /** Performs the actual option parsing. */
  virtual bool operator()(const char *argv[], int argc, size_t &idx, Parser &prs);
  /** Generates a string representation for the option rule. */
  virtual std::string dump();

protected:
  /** The short name (single charater) of the option. */
  char _short_name;
  /** Identifier of the option. */
  std::string _id;
  /** The long name for this option. */
  std::string _long_name;
  /** If this is true the option does not accept a value, it is a flag. */
  bool _is_flag;
  friend class Parser;
};


class KeywordRule: public RuleInterface
{
protected:
  KeywordRule(Parser *parser, const std::string &keyword);

public:
  virtual ~KeywordRule();

  /** Tries to parse the option rule, returns true on success. */
  virtual bool operator()(const char *argv[], int argc, size_t &idx, Parser &parser);

  /** Dumps the rule expression. */
  virtual std::string dump();

protected:
  std::string _keyword;
  friend class Parser;
};

/** Consumes exactly on option rule from the owned set of options.
 * This class can be used to express conflicting options.
 * @ingroup optionparser
 */
class OneOfRule: public RuleInterface
{
protected:
  /** \todo Is this needed? */
  OneOfRule(Parser *parser);

  /** The constructor takes a list of option rules. The ownership of these rules is
   * tranferred to this object and they are destroyed if this object is destroyed. */
  OneOfRule(Parser *parser, std::list<RuleInterface *> &options);

  /** The constructor takes the number of optional rules and a list of @c OptionRuleInterface
   * instances. */
  OneOfRule(Parser *parser, size_t n, ...);


public:
  /** Destructor. Destroies also the encapsulated option rules. */
  virtual ~OneOfRule();

  /** Tries to parse the option rule, returns true on success. */
  virtual bool operator()(const char *argv[], int argc, size_t &idx, Parser &parser);

  /** Dumps the rule expression. */
  virtual std::string dump();

protected:
  /** Holds a list of the options that one has to match. */
  std::list<RuleInterface *> options;

  friend class Parser;
  friend class RuleInterface;
};



/**
 * Consumes any number of the enclosed option rules to be applied.
 * @ingroup optionparser
 */
class ZeroOrMoreRule: public RuleInterface
{
protected:
  /** Constructor. The ownership of the given rule is transferred to this object and will be
   * destroyed if this object gets destroyed. */
  ZeroOrMoreRule(Parser *parser, RuleInterface *rule);

public:
  /** Destructor. Also destroies the encapsulated option rule. */
  virtual ~ZeroOrMoreRule();

  virtual bool operator()(const char *argv[], int argc, size_t &idx, Parser &parser);

  virtual std::string dump();

protected:
  /** Holds the encapsulated option rule. */
  RuleInterface *rule;

  friend class Parser;
};



/**
 * Represents a value of an option.
 * @ingroup optionparser
 */
class ValueRule: public RuleInterface
{
protected:
  /** Hidden constructor, use @c Parser::Value to obtain an instance. */
  ValueRule(Parser *parser, const std::string &identifier="");

public:
  /** Destructor. */
  virtual ~ValueRule();

  virtual bool operator()(const char *argv[], int argc, size_t &idx, Parser &parser);
  virtual std::string dump();

  friend class Parser;

protected:
  std::string _id;
};



/**
 * Represents a list of option rules that all have to match the input in the given order.
 * @ingroup optionparser
 */
class ListRule: public RuleInterface
{
protected:
  /** Constructor. The ownership of the options given is transferred to this object. They
   * will be destroyed if this object is destroyed. */
  ListRule(Parser *parser, std::list<RuleInterface *> &opts);

  /**
   * Hidden constructor with variable arguments.
   */
  ListRule(Parser *parser, size_t n, ...);


public:
  /** Destructor. */
  virtual ~ListRule();

  virtual bool operator()(const char *argv[], int argc, size_t &idx, Parser &parser);

  virtual std::string dump();

protected:
  /** Holds the list of options. */
  std::list<RuleInterface *> options;

  friend class Parser;
  friend class RuleInterface;
};



/**
 * An optional rule.
 * @ingroup optionparser
 */
class OptionalRule: public RuleInterface
{
protected:
  /** The constructor will take the ownership of the given option. */
  OptionalRule(Parser *parser, RuleInterface *rule);

public:
  /** The destructor. */
  virtual ~OptionalRule();

  virtual bool operator()(const char *argv[], int argc, size_t &idx, Parser &parser);

  virtual std::string dump();

  friend class Parser;

protected:
  /**
   * The optional option.
   */
  RuleInterface *rule;
};



/**
 * The parser class encapsulating all option rules.
 *
 * The following example shows how to use the option parser for a simple case:
 *
 * \code
 * int main(char *argv[], int argc)
 * {
 *   Parser parser(
 *      Parser::opt(Parser::Flag("help") | Parser::Flag("version")) );
 *
 *   if (! parser(argv, argc)) {
 *     std::cerr << parser.format_help(argv[0]);
 *     return -1;
 *   }
 *
 *   return 0;
 * }
 * \endcode
 *
 * In this example, there are two flags, that exclude eachother but they are optional.
 * This means you could call with --help, with --version or without any flag. But to call
 * the program with --help and --version flag would be invalid.
 *
 * The example above can be extended with an mandory argument:
 * \code
 * Parser parser(
 *    (Parser::opt(Parser::Flag("help") | Parser::Flag("version")), Parser::Value()) );
 * \endcode
 *
 * @ingroup optionparser
 */
class Parser
{
public:
  /** Constructs a parser w/o start rule. Use @c setGrammar to set the grammar before parsing. */
  Parser();
  /** Destroies also the start-rule of the option grammar. */
  ~Parser();

  /** Sets the grammar (start-rule). */
  void setGrammar(RuleInterface &rule);
  /** Actually initializing the option parsing. */
  bool parse(const char *argv[], int argc);
  /** Formats a help/usage string. */
  std::string format_help(std::string prg_name);
  /** Sets a flag specified by its long name to be present. This method is called by the
   * option rules. */
  void set_flag(const std::string &name);
  /** Returns true if the flag has been found while parsing. */
  bool has_flag(const std::string &name);
  /** Sets the value of the option. This method is called by the option rule. */
  void set_option(const std::string &name, const std::string &value);
  /** Returns true if the option was present. */
  bool has_option(std::string name);
  /** Returns the value of the option. */
  const std::list<std::string> &get_option(const std::string &name);
  /** Adds a argument. */
  void add_value(const std::string &identifier, const std::string &value);
  /** Returns @c true if there are some values given for the specified identifier. */
  bool has_values(const std::string &identifier) const;
  /** Returns the list of arguments found while parsing. */
  const std::list<std::string> &get_values(const std::string &identifier);
  /** Retunrs @c true if the given keyword is present. */
  bool has_keyword(const std::string &keyword) const;
  /** Adds a keyword to the set of present keywords. */
  void add_keyword(const std::string &keyword);

protected:
  /** Holds the start-rule of the option grammar.*/
  RuleInterface *_rule;
  /** A set of all long option names of present flags. */
  std::set<std::string> _present_flags;
  /** A map "long name" -> "values" of all present options. */
  std::map<std::string, std::list<std::string> > _present_options;
  /** A list holding all arguments present. */
  std::map<std::string, std::list<std::string> > _present_values;
  /** A set of present keywords. */
  std::set<std::string> _present_keywords;
  /** Holds the rules of the parser. */
  std::set<RuleInterface *> _rules;

protected:
  /** Adds a rule to the list of owned rules. */
  void addRule(RuleInterface *rule);
  friend class RuleInterface;

public:
  /** Constructs a @c OptionRule as a flag. */
  inline RuleInterface &Flag(const std::string &name, char short_name=0) {
    RuleInterface *rule = new OptionRule(this, name, name, true, short_name);
    return *rule;
  }

  inline RuleInterface &Flag(const std::string &identifier, const std::string &name, char short_name=0) {
    RuleInterface *rule = new OptionRule(this, identifier, name, true, short_name);
    return *rule;
  }

  /** Constructs a @c OptionRule as an option. */
  inline RuleInterface &Option(const std::string &name, char short_name=0) {
    RuleInterface *rule = new OptionRule(this, name, name, false, short_name);
    return *rule;
  }

  /** Constructs a @c OptionRule as an option. */
  inline RuleInterface &Option(const std::string &identifier, const std::string &name, char short_name=0) {
    RuleInterface *rule = new OptionRule(this, identifier, name, false, short_name);
    return *rule;
  }

  /** Constructs a @c ValueRule. */
  inline RuleInterface &Value(const std::string &identifier="") {
    RuleInterface *rule = new ValueRule(this, identifier);
    return *rule;
  }

  /** Constructs a @c KeywordRule. */
  inline RuleInterface &Keyword(const std::string &identifier) {
    RuleInterface *rule = new KeywordRule(this, identifier);
    return *rule;
  }

  /** Constructs a @c OptionalRule. */
  inline RuleInterface &opt(RuleInterface &rule) {
    RuleInterface *newrule = new OptionalRule(this, &rule);
    return *newrule;
  }

  /** Constructs a @c ZeroOrMoreRule. */
  inline RuleInterface &zeroOrMore(RuleInterface &rule) {
    RuleInterface *newrule = new ZeroOrMoreRule(this, &rule);
    return *newrule;
  }
};


}
}

#endif
