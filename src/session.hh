#ifndef SESSION_HH
#define SESSION_HH

#include "application.hh"
#include <H5Cpp.h>

class Item;

/** The "namespace" providing functions for serializing and loading of sessions. */
class Session
{
public:
  static bool save(const QString &filename, Application *app);
  static bool load(const QString &filename, Application *app);

protected:
  static Item *loadItem(const std::string &objname, H5::DataSet &dataset);
  static bool saveItem(H5::H5File &file, Item *item);
  static Item *loadTimeseriesItem(const std::string &objname, H5::DataSet &dataset);
  static bool saveTimeseriesItem(H5::H5File &file, TimeseriesItem *item);
   static Item *loadTransformedItem(const std::string &objname, H5::DataSet &dataset);
  static bool saveTransformedItem(H5::H5File &file, TransformedItem *item);

  static bool getAttribute(H5::DataSet &dataset, const std::string &name, unsigned int &value);
  static bool setAttribute(H5::DataSet &dataset, const std::string &name, unsigned int value);
  static bool getAttribute(H5::DataSet &dataset, const std::string &name, double &value);
  static bool setAttribute(H5::DataSet &dataset, const std::string &name, double value);
  static bool getAttribute(H5::DataSet &dataset, const std::string &name, Eigen::VectorXd &value);
  static bool setAttribute(H5::DataSet &dataset, const std::string &name, const Eigen::Ref<const Eigen::VectorXd> &value);
  static bool readArray(H5::DataSet &dataset, Eigen::VectorXd &value);
  static bool readArray(H5::DataSet &dataset, Eigen::VectorXcd &value);
  static bool readArray(H5::DataSet &dataset, Eigen::MatrixXcd &value);
};

#endif // SESSION_HH
