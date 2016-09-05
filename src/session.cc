#include "session.hh"
#include "utils/logger.hh"
#include "timeseriesitem.hh"
#include "transformeditem.hh"

typedef enum {
  WAVELET_MORLET = 0,
  WAVELET_CAUCHY
} WaveletType;

typedef enum {
  ITEM_TIMESERIES = 0,
  ITEM_TRANSFORMED
} ItemType;

bool
Session::load(const QString &filename, Application *app) {
  if (! H5::H5File::isHdf5(filename.toUtf8().constData())) {
    logError() << "File " << filename.toUtf8().constData()
               << " is not a proper HDF5 file.";
    return false;
  }

  // Open file
  H5::H5File file;
  try { file.openFile(filename.toUtf8().constData(), H5F_ACC_RDONLY); }
  catch (H5::FileIException error) {
    logError() << "Cannot load session from " << filename.toStdString()
               << ": " << error.getDetailMsg();
    return false;
  }

  // Iterate through objects in file.
  size_t nObj = file.getNumObjs();
  for (size_t i=0; i<nObj; i++) {
    // Skip non-datasets
    if (H5O_TYPE_DATASET != file.childObjType(i))
      continue;
    // open dataset
    H5::DataSet dataset = file.openDataSet(file.getObjnameByIdx(i));
    Item *item = loadItem(dataset);
    if (item)
      app->items()->addItem(item);
  }

  return true;
}

bool
Session::save(const QString &filename, Application *app) {
  H5::H5File file;
  try { file.openFile(filename.toUtf8().constData(), H5F_ACC_CREAT); }
  catch (H5::FileIException error) {
    logError() << "Cannot save session to " << filename.toUtf8().constData()
               << ": " << error.getCDetailMsg();
    return false;
  }

  bool success = false;
  for (int i=0; i<app->items()->rowCount(QModelIndex()); i++)
    success = success || saveItem(file, app->items()->item(i));
  file.flush(H5F_SCOPE_GLOBAL);
  file.close();
  return success;
}

Item *
Session::loadItem(H5::DataSet &dataset) {
  // Check if dataset has type attribute
  uint type;
  if (! getAttribute(dataset, "type", type))
    return 0;
  if (ITEM_TIMESERIES == type)
    return loadTimeseriesItem(dataset);
  if (ITEM_TRANSFORMED == type)
    return loadTransformedItem(dataset);

  logError() << "Cannot load item " << dataset.getObjName()
             << ": Unknown type " << type;
  return 0;
}

bool
Session::saveItem(H5::H5File &file, Item *item) {
  if (TimeseriesItem *titem = dynamic_cast<TimeseriesItem *>(item))
    return saveTimeseriesItem(file, titem);
  if (TransformedItem *titem = dynamic_cast<TransformedItem *>(item))
    return saveTransformedItem(file, titem);
  return false;
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
  if (H5T_FLOAT != attrFs.getTypeClass())
    return 0;
  double Fs = 1.0;
  attrFs.read(H5::PredType::NATIVE_DOUBLE, &Fs);

  // Check if dataset is double array
  if (H5T_ARRAY == dataset.getTypeClass()) {
    logError() << "Cannot load timeseries " << dataset.getObjName()
               << ": Unexpected type. Expected double array.";
    return 0;
  }
  // Get array dimensions
  H5::ArrayType datasetType = dataset.getArrayType();
  if (1 != datasetType.getArrayNDims()) {
    logError() << "Cannot load timeseries " << dataset.getObjName()
               << ": Unexpected rank-" << datasetType.getArrayNDims() << ", Expected rank-1.";
    return 0;
  }
  hsize_t Nsam=0;
  datasetType.getArrayDims(&Nsam);
  Eigen::VectorXd data(Nsam);
  dataset.read(data.data(), H5::PredType::NATIVE_DOUBLE);

  return new TimeseriesItem(data, Fs, QString::fromStdString(dataset.getObjName()));
}

bool
Session::saveTimeseriesItem(H5::H5File &file, TimeseriesItem *item) {
  hsize_t dims[1] = { hsize_t(item->data().size()) };
  H5::DataSpace fspace(1, dims);
  H5::DataSet dataset = file.createDataSet(
        item->label().toStdString(), H5::PredType::NATIVE_DOUBLE, fspace);
  dataset.write(item->data().data(), H5::PredType::NATIVE_DOUBLE);
  setAttribute(dataset, "type", uint(ITEM_TIMESERIES));
  setAttribute(dataset, "Fs", item->Fs());
  return true;
}

Item *
Session::loadTransformedItem(H5::DataSet &dataset) {
  double Fs = 1;
  uint waveletId;
  uint scalingId;
  Eigen::VectorXd scales;

  if (! getAttribute(dataset, "Fs", Fs)) {
    logError() << "Cannot load transformed " << dataset.getObjName()
               << ": No valid sample rate attribute set.";
    return 0;
  }
  if (! getAttribute(dataset, "scaling", scalingId)) {
    logError() << "Cannot load transformed " << dataset.getObjName()
               << ": No valid scaling attribute set.";
    return 0;
  }
  if (! getAttribute(dataset, "scales", scales)) {
    logError() << "Cannot load transformed " << dataset.getObjName()
               << ": No valid scales attribute set.";
    return 0;
  }

  // Dispatch by type
  if (! getAttribute(dataset, "wavelet", waveletId)) {
    logError() << "Cannot load transformed " << dataset.getObjName()
               << ": No valid wavelet attribute set.";
    return 0;
  }
  wt::Wavelet wavelet;
  if (WAVELET_MORLET == waveletId) {
    double dff;
    if (! getAttribute(dataset, "dff", dff)) {
      logError() << "Cannot load transformed " << dataset.getObjName()
                 << ": No valid wavelet parameter set.";
      return 0;
    }
    wavelet = wt::Morlet(dff);
  } else if (WAVELET_CAUCHY == waveletId) {
    double alpha;
    if (! getAttribute(dataset, "alpha", alpha)) {
      logError() << "Cannot load transformed " << dataset.getObjName()
                 << ": No valid wavelet parameter set.";
      return 0;
    }
    wavelet = wt::Cauchy(alpha);
  } else {
    logError() << "Cannot load transformed " << dataset.getObjName()
               << ": Unknown wavelet type ID " << uint(waveletId) << ".";
    return 0;
  }

  Eigen::MatrixXcd data;
  if (! readArray(dataset, data)) {
    logError() << "Cannot load transformed " << dataset.getObjName()
               << ": Invalid data.";
    return 0;
  }

  return new TransformedItem(wavelet, Fs, scales, TransformedItem::Scaling(scalingId), data);
}

bool
Session::saveTransformedItem(H5::H5File &file, TransformedItem *item) {
  H5::CompType ctype;
  ctype.insertMember("re", 0, H5::PredType::NATIVE_DOUBLE);
  ctype.insertMember("im", sizeof(double), H5::PredType::NATIVE_DOUBLE);

  hsize_t dims[2] = { hsize_t(item->data().cols()), hsize_t(item->data().rows()) };
  H5::DataSpace fspace(2, dims);
  H5::DataSet dataset = file.createDataSet(
        item->label().toStdString(), ctype, fspace);
  dataset.write(item->data().data(), ctype);
  setAttribute(dataset, "type", uint(ITEM_TRANSFORMED));
  setAttribute(dataset, "Fs", item->Fs());
  setAttribute(dataset, "scaling", uint(item->scaling()));
  setAttribute(dataset, "scales", item->scales());
  if (item->wavelet().is<wt::Morlet>()) {
    setAttribute(dataset, "wavelet", uint(WAVELET_MORLET));
    setAttribute(dataset, "dff", item->wavelet().as<wt::Morlet>().dff());
  } else if (item->wavelet().is<wt::Cauchy>()) {
    setAttribute(dataset, "wavelet", uint(WAVELET_CAUCHY));
    setAttribute(dataset, "dff", item->wavelet().as<wt::Cauchy>().alpha());
  }
  return true;
}

bool
Session::getAttribute(H5::DataSet &dataset, const std::string &name, unsigned int &value) {
  if (! dataset.attrExists(name))
    return false;

  H5::Attribute attr = dataset.openAttribute(name);
  if (H5T_INTEGER != attr.getTypeClass())
    return false;

  try { attr.read(H5::PredType::NATIVE_UINT, &value); }
  catch (H5::AttributeIException error) {
    logError() << "Cannot read attribue " << name
               << " from " << dataset.getObjName()
               << ": " << error.getCDetailMsg();
    return false;
  }
  attr.close();

  return true;
}

bool
Session::setAttribute(H5::DataSet &dataset, const std::string &name, unsigned int value) {
  H5::DataSpace fspace(H5S_SCALAR);
  H5::Attribute attr = dataset.createAttribute(name, H5::PredType::NATIVE_UINT, fspace);
  attr.write(H5::PredType::NATIVE_UINT, &value);
  attr.close();
  return true;
}

bool
Session::getAttribute(H5::DataSet &dataset, const std::string &name, double &value) {
  if (! dataset.attrExists(name))
    return false;

  H5::Attribute attr = dataset.openAttribute(name);
  if (H5T_FLOAT != attr.getTypeClass())
    return false;

  try { attr.read(H5::PredType::NATIVE_DOUBLE, &value); }
  catch (H5::AttributeIException error) {
    logError() << "Cannot read attribue " << name
               << " from " << dataset.getObjName()
               << ": " << error.getCDetailMsg();
    return false;
  }
  attr.close();

  return true;
}

bool
Session::setAttribute(H5::DataSet &dataset, const std::string &name, double value) {
  H5::DataSpace fspace(H5S_SCALAR);
  H5::Attribute attr = dataset.createAttribute(name, H5::PredType::NATIVE_DOUBLE, fspace);
  attr.write(H5::PredType::NATIVE_DOUBLE, &value);
  attr.close();
  return true;
}

bool
Session::getAttribute(H5::DataSet &dataset, const std::string &name, Eigen::Ref<Eigen::VectorXd> value) {
  if (! dataset.attrExists(name))
    return false;

  H5::Attribute attr = dataset.openAttribute(name);
  if (H5T_ARRAY != attr.getTypeClass())
    return false;

  // Get scales dimensions
  H5::ArrayType type = attr.getArrayType();
  if (1 != type.getArrayNDims()) {
    logError() << "Cannot load attribute " << name
               << ": Unexpected rank-" << type.getArrayNDims() << ", Expected rank-1.";
    return 0;
  }

  hsize_t N=0;
  type.getArrayDims(&N);
  value.resize(N);
  dataset.read(value.data(), H5::PredType::NATIVE_DOUBLE);

  return true;
}

bool
Session::setAttribute(H5::DataSet &dataset, const std::string &name, const Eigen::Ref<const Eigen::VectorXd> &value) {
  hsize_t dims[1] = { hsize_t(value.size()) };
  H5::DataSpace fspace(1, dims);
  H5::Attribute attr = dataset.createAttribute(name, H5::PredType::NATIVE_DOUBLE, fspace);
  attr.write(H5::PredType::NATIVE_DOUBLE, value.data());
  attr.close();
  return true;
}

bool
Session::readArray(H5::DataSet &dataset, Eigen::Ref<Eigen::VectorXd> value) {
  if (H5T_ARRAY != dataset.getTypeClass())
    return false;

  H5::ArrayType type = dataset.getArrayType();
  if (1 != type.getArrayNDims()) {
    logError() << "Cannot read vector " << dataset.getObjName()
               << ": Unexpected rank-" << type.getArrayNDims() << ", Expected rank-1.";
    return 0;
  }

  hsize_t N=0;
  type.getArrayDims(&N);
  value.resize(N);
  dataset.read(value.data(), H5::PredType::NATIVE_DOUBLE);

  return true;
}

bool
Session::readArray(H5::DataSet &dataset, Eigen::Ref<Eigen::MatrixXcd> value) {
  if (H5T_ARRAY != dataset.getTypeClass())
    return false;
  H5::ArrayType type = dataset.getArrayType();
  if (2 != type.getArrayNDims()) {
    logError() << "Cannot read matrix " << dataset.getObjName()
               << ": Unexpected rank-" << type.getArrayNDims() << ", Expected rank-2.";
    return 0;
  }
  hsize_t N[2];
  type.getArrayDims(N);
  value.resize(N[1],N[0]);
  H5::CompType complexType;
  complexType.insertMember("re", 0, H5::PredType::NATIVE_DOUBLE);
  complexType.insertMember("im", sizeof(double), H5::PredType::NATIVE_DOUBLE);
  dataset.read(value.data(), complexType);

  return true;
}
