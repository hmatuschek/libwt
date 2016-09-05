#include "session.hh"
#include "utils/logger.hh"
#include "timeseriesitem.hh"
#include "transformeditem.hh"

bool
Session::load(const QString &filename, Application *app) {
  if (! H5::H5File::isHdf5(filename.toUtf8().constData())) {
    logError() << "File " << filename.toUtf8().constData()
               << " is not a proper HDF5 file.";
    return false;
  }

  // Open file
  H5::H5File file(filename.toUtf8().constData(), );

  // Check if file has an attribute called "wtoolsession" to identify serialized sessions.
  if (!file.attrExists("wtoolsession")) {
    logError() << "File " << filename.toUtf8().constData()
               << " does not contain a serialized wtool session.";
    return false;
  }

  // Iterate through objects in file.
  size_t nObj = file.numObjs();
  for (size_t i=0; i<nObj; i++) {
    if (DATASET != file.getObjTypeByIdx(i))
      continue;
    H5::DataSet dataset = file.openDataSet(file.getObjnameByIdx(i));
    if (! dataset.attrExists("type"))
      continue;
    Item *item = loadItem(dataset);
    if (item)
      app->items()->addItem(item);
  }
}

Item *
Session::loadItem(H5::DataSet &dataset) {
  H5::Attribute type = dataset.openAttribute("type");
  if ("StrType" != type.getDataType().getClass())
    return false;
  size_t size = type.getInMemDataSize();
  char buffer[typeSize];
  type.read(H5T_STRING, buffer);
  if ("")
  /// @todo Dispatch by type
}

Item *
Session::loadTimeseriesItem(H5::DataSet &dataset) {
  // Check if dataset has Fs attribute
  if (! dataset.attrExists("Fs")) {
    logError() << "Cannot load timeseries " << dataset.getObjName()
               << ": No sample rate attribute set.";
    return 0;
  }
  H5::Attribute attrFs = dataset.openAttribute("Fs");
  /// @todo Check if attrFs.getDataType() is double
  double Fs = 1.0;
  attrFs.read(H5T_NATIVE_DOUBLE, &Fs);

  // Check if dataset is double array
  if (true) {
    logError() << "Cannot load timeseries " << dataset.getObjName()
               << ": Unexpected type. Expected double array.";
    return 0;
  }
  // Get array dimensions
  H5::ArrayType dataType = dataset.getArrayType();
  if (1 != dataType.getArrayNDims()) {
    logError() << "Cannot load timeseries " << dataset.getObjName()
               << ": Unexpected rank-" << dataType.getArrayNDims() << ", Expected rank-1.";
    return 0;
  }

  QString label = QString::fromStdString(dataset.getObjName());

}
