#ifndef SESSION_HH
#define SESSION_HH

#include "application.hh"
#include <H5Cpp.h>

class Item;

/** The "namespace" providing functions for serializing and loading of sessions. */
class Session
{
public:
  static bool save(const QString &filename);
  static load(const QString &filename, Application *app);

protected:
  Item *loadItem(H5::DataSet &dataset);
  Item *loadTimeseriesItem(H5::DataSet &dataset);
  Item *loadTransformedItem(H5::DataSet &dataset);

};

#endif // SESSION_HH
