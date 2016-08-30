#ifndef __WT_LOGGER_HH__
#define __WT_LOGGER_HH__

#include <sstream>
#include <iostream>
#include <string>
#include <ctime>
#include <list>

#include "../api.hh"

namespace wt {

/** A log message.
 * @ingroup api */
class LogMessage
{
public:
  /** Specifies possible log levels. */
  typedef enum {
    LDEBUG = 0,  ///< Debug information.
    LINFO,       ///< Runtime & status information.
    LWARNING,    ///< Warnings and minor issues.
    LERROR       ///< Errors and other major issues.
  } Level;

public:
  /** Empty constructor. */
  LogMessage();
  /** Constructor from @c filename, @c line, log @c level and @c message text. */
  LogMessage(const std::string &filename, int line, Level level, const std::string &message);
  /** Copy constructor. */
  LogMessage(const LogMessage &other);
  /** Assignment operator. */
  LogMessage &operator=(const LogMessage &other);

  /** Returns the filename, where the message originated. */
  const std::string &filename() const;
  /** Returns the line number, where the message originated. */
  int linenumber() const;
  /** Returns the level of the message. */
  Level level() const;
  /** Returns the actual message. */
  const std::string &message() const;
  /** Returns the timestamp of the message. */
  const std::time_t &timestamp() const;

protected:
  /** The filename where the message originated. */
  std::string _filename;
  /** The line number where the message originated. */
  int _line;
  /** The message level. */
  Level _level;
  /** The actual message. */
  std::string _message;
  /** The timestamp of the message. */
  std::time_t _timestamp;
};


/** A stream object assembling a log message. Upon destruction, a log message gets assembled and
 * passed to the logger. */
class LogMessageStream: public std::ostringstream
{
public:
  /** Constructs a ne log message stream at the position @c filename & @c line with the specified
   * log @c level. Usually the @c logDebug, @c logInfo, @c logWarning or @c logError macros are
   * used to instantiate a @c LogMessageStream. */
  LogMessageStream(const std::string &filename, int line, LogMessage::Level level);
  /** Copy constructor. */
  LogMessageStream(const LogMessageStream &other);
  /** Destructor. Upon destruction, a log message will be assembled and send to the @c Logger
   * instance. */
  virtual ~LogMessageStream();

  /** Serializes a container. */
  LogMessageStream &operator<< (const double &value);

protected:
  /** The name of the file where the message originated. */
  std::string _filename;
  /** The line where the message originated. */
  int _line;
  /** Level of the log message. */
  LogMessage::Level _level;
};


/** The base class of all log-handlers. */
class LogHandlerObj: public Object
{
protected:
  /** Hidden constructor. */
  LogHandlerObj(LogMessage::Level level);

public:
  /** Destructor. */
  virtual ~LogHandlerObj();

  void mark();

  /** Needs to be implemented to handle log messages. */
  virtual void handleMessage(const LogMessage &msg) = 0;

protected:
  /** Minimum log level to process. */
  LogMessage::Level _minLevel;
};


/** Base class of all log handlers.
 * @ingroup api*/
class LogHandler: public Container
{
public:
  /** Object type of the container. */
  typedef LogHandlerObj ObjectType;

public:
  /** Constructor. */
  LogHandler(LogHandlerObj *obj);
  /** Copy constructor. */
  LogHandler(const LogHandler &other);
  /** Destructor. */
  virtual ~LogHandler();
  /** Assignment operator. */
  LogHandler &operator=(const LogHandler &other);

  /** Processes a messages. */
  void handleMessage(const LogMessage &msg);

protected:
  /** A reference to the log handler instance. */
  LogHandlerObj *_loghandler;
};


/** Serializes log messages to the given file. */
class IOLogHandlerObj: public LogHandlerObj
{
public:
  /** Constructor.
   * @param level Spicifies the mimimum log level to process.
   * @param stream Specifies the output stream. */
  IOLogHandlerObj(std::ostream &stream=std::cerr, LogMessage::Level level=LogMessage::LDEBUG);

  /** Implements the @c LogHandler interface. */
  void handleMessage(const LogMessage &msg);

protected:
  /** A textstream to serialize into. */
  std::ostream &_stream;
};


/** Serializes log messages to the given stream.
 * @ingroup api */
class IOLogHandler: public LogHandler
{
public:
  /** Object type of the container. */
  typedef IOLogHandlerObj ObjectType;

public:
  /** Constructs a new log handler for the to the given stream.
   * @param level Spicifies the mimimum log level to process.
   * @param stream Specifies the output stream. */
  IOLogHandler(std::ostream &stream=std::cerr, LogMessage::Level level=LogMessage::LDEBUG);
  /** Copy constructor. */
  IOLogHandler(const IOLogHandler &other);
  /** Assignement operator. */
  IOLogHandler &operator=(const IOLogHandler &other);
};


/** A singleton logger class.
 * @ingroup api */
class Logger
{
protected:
  /** Hidden constructor. */
  Logger();

public:
  /** Destructor. */
  virtual ~Logger();

  /** Logs a message. */
  static void log(const LogMessage &msg);
  /** Adds a handler to the logger, the ownership is transferred to the @c Logger. */
  static void addHandler(const LogHandler &handler);

protected:
  /** Factory method. */
  static Logger *get();

protected:
  /** The list of registered handlers. */
  std::list<LogHandler> _handler;

protected:
  /** The singleton instance. */
  static Logger *_instance;
};

}

/** Convenience macro to create and submit a @c LogMessage with level "DEBUG".
 * @ingroup api */
#define logDebug()   (wt::LogMessageStream(__FILE__, __LINE__, wt::LogMessage::LDEBUG))
/** Convenience macro to create and submit a @c LogMessage with level "INFO".
 * @ingroup api */
#define logInfo()    (wt::LogMessageStream(__FILE__, __LINE__, wt::LogMessage::LINFO))
/** Convenience macro to create and submit a @c LogMessage with level "Warning".
 * @ingroup api */
#define logWarning() (wt::LogMessageStream(__FILE__, __LINE__, wt::LogMessage::LWARNING))
/** Convenience macro to create and submit a @c LogMessage with level "ERROR".
 * @ingroup api */
#define logError()   (wt::LogMessageStream(__FILE__, __LINE__, wt::LogMessage::LERROR))

#endif // __WT_LOGGER_HH__
