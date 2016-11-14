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
    if (H5G_DATASET != file.getObjTypeByIdx(i))
      continue;
    std::string objname = file.getObjnameByIdx(i);
    // open dataset
    H5::DataSet dataset = file.openDataSet(objname);
    Item *item = loadItem(objname, dataset);
    if (item)
      app->items()->addItem(item);
  }

  return true;
}

bool
Session::save(const QString &filename, Application *app) {
  try {
    H5::H5File file(filename.toUtf8().constData(), H5F_ACC_TRUNC);
    bool success = false;
    for (int i=0; i<app->items()->rowCount(QModelIndex()); i++) {
      success = saveItem(file, app->items()->item(i)) || success;
    }
    file.flush(H5F_SCOPE_GLOBAL);
    file.close();
    return success;
  } catch (H5::FileIException error) {
    logError() << "Cannot save session to " << filename.toUtf8().constData()
               << ": " << error.getCDetailMsg();
    return false;
  }
}

Item *
Session::loadItem(const std::string &objname, H5::DataSet &dataset) {
  // Check if dataset has type attribute
  uint type;
  if (! getAttribute(dataset, "type", type))
    return 0;
  if (ITEM_TIMESERIES == type)
    return loadTimeseriesItem(objname, dataset);
  if (ITEM_TRANSFORMED == type)
    return loadTransformedItem(objname, dataset);

  logError() << "Cannot load item " << objname
             << ": Unknown type " << type;
  return 0;
}

bool
Session::saveItem(H5::H5File &file, Item *item) {
  if (TimeseriesItem *titem = dynamic_cast<TimeseriesItem *>(item))
    return saveTimeseriesItem(file, titem);
  if (TransformedItem *titem = dynamic_cast<TransformedItem *>(item))
    return saveTransformedItem(file, titem);
  logDebug() << "Item " << item->label().toUtf8().constData() << " not serialized.";
  return false;
}

Item *
Session::loadTimeseriesItem(const std::string &objname, H5::DataSet &dataset) {
  // Check if dataset has Fs attribute
  double Fs;
  if (! getAttribute(dataset, "Fs", Fs)) {
    logError() << "Cannot load timeseries " << objname
               << ": No sample rate attribute set.";
    return 0;
  }

  // Get data
  Eigen::VectorXd data;
  if (! readArray(dataset, data)) {
    logError() << "Cannot load timeseries " << objname << ".";
    return 0;
  }
  QString label = QString::fromStdString(objname);
  if (label.startsWith("/")) label.remove("/");
  return new TimeseriesItem(data, Fs, label);
}

bool
Session::saveTimeseriesItem(H5::H5File &file, TimeseriesItem *item) {
  logDebug() << "Save timeseries " << item->label().toStdString() << "...";
  hsize_t dims[1] = { hsize_t(item->data().size()) };
  H5::DataSpace fspace(1, dims);
  H5::DataSet dataset = file.createDataSet(
        item->label().toStdString(), H5::PredType::NATIVE_DOUBLE, fspace);
  dataset.write(item->data().data(), H5::PredType::NATIVE_DOUBLE);
  setAttribute(dataset, "type", uint(ITEM_TIMESERIES));
  setAttribute(dataset, "Fs", item->Fs());
  fspace.close(); dataset.close();
  file.flush(H5F_SCOPE_GLOBAL);
  return true;
}

Item *
Session::loadTransformedItem(const std::string &objname, H5::DataSet &dataset) {
  double Fs = 1;
  uint waveletId;
  uint scalingId;
  Eigen::VectorXd scales;

  if (! getAttribute(dataset, "Fs", Fs)) {
    logError() << "Cannot load transformed " << objname
               << ": No valid sample rate attribute set.";
    return 0;
  }
  if (! getAttribute(dataset, "scaling", scalingId)) {
    logError() << "Cannot load transformed " << objname
               << ": No valid scaling attribute set.";
    return 0;
  }
  if (! getAttribute(dataset, "scales", scales)) {
    logError() << "Cannot load transformed " << objname
               << ": No valid scales attribute set.";
    return 0;
  }

  // Dispatch by type
  if (! getAttribute(dataset, "wavelet", waveletId)) {
    logError() << "Cannot load transformed " << objname
               << ": No valid wavelet attribute set.";
    return 0;
  }
  wt::Wavelet wavelet;
  if (WAVELET_MORLET == waveletId) {
    double dff;
    if (! getAttribute(dataset, "dff", dff)) {
      logError() << "Cannot load transformed " << objname
                 << ": No valid wavelet parameter set.";
      return 0;
    }
    wavelet = wt::Morlet(dff);
  } else if (WAVELET_CAUCHY == waveletId) {
    double alpha;
    if (! getAttribute(dataset, "alpha", alpha)) {
      logError() << "Cannot load transformed " << objname
                 << ": No valid wavelet parameter set.";
      return 0;
    }
    wavelet = wt::Cauchy(alpha);
  } else {
    logError() << "Cannot load transformed " << objname
               << ": Unknown wavelet type ID " << uint(waveletId) << ".";
    return 0;
  }

  Eigen::MatrixXcd data;
  if (! readArray(dataset, data)) {
    logError() << "Cannot load transformed " << objname
               << ": Invalid data.";
    return 0;
  }

  QString label = QString::fromStdString(objname);
  if (label.startsWith("/")) label.remove("/");
  return new TransformedItem(wavelet, Fs, scales, TransformedItem::Scaling(scalingId), data, label);
}

bool
Session::saveTransformedItem(H5::H5File &file, TransformedItem *item) {
  logDebug() << "Save transformed " << item->label().toStdString() << "...";
  H5::CompType ctype(sizeof(std::complex<double>));
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
    setAttribute(dataset, "alpha", item->wavelet().as<wt::Cauchy>().alpha());
  }
  fspace.close(); dataset.close();
  file.flush(H5F_SCOPE_GLOBAL);
  return true;
}

bool
Session::getAttribute(H5::DataSet &dataset, const std::string &name, unsigned int &value) {
  if (0 == H5Aexists(dataset.getId(), name.c_str()))
    return false;

  H5::Attribute attr = dataset.openAttribute(name);
  if (H5T_INTEGER != attr.getTypeClass())
    return false;

  try { attr.read(H5::PredType::NATIVE_UINT, &value); }
  catch (H5::AttributeIException error) {
    logError() << "Cannot read attribue " << name
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
  fspace.close(); attr.close();
  return true;
}

bool
Session::getAttribute(H5::DataSet &dataset, const std::string &name, double &value) {
  if (0 == H5Aexists(dataset.getId(), name.c_str()))
    return false;

  H5::Attribute attr = dataset.openAttribute(name);
  if (H5T_FLOAT != attr.getTypeClass())
    return false;

  try { attr.read(H5::PredType::NATIVE_DOUBLE, &value); }
  catch (H5::AttributeIException error) {
    logError() << "Cannot read attribue " << name
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
  fspace.close(); attr.close();
  return true;
}

bool
Session::getAttribute(H5::DataSet &dataset, const std::string &name, Eigen::VectorXd &value) {
  if (0 == H5Aexists(dataset.getId(), name.c_str()))
    return false;

  H5::Attribute attr = dataset.openAttribute(name);
  if (H5T_FLOAT != attr.getTypeClass())
    return false;

  // Get scales dimensions
  H5::DataSpace space = attr.getSpace();
  if (1 != space.getSimpleExtentNdims()) {
    logError() << "Cannot load attribute " << name
               << ": Unexpected rank-" << space.getSimpleExtentNdims() << ", Expected rank-1.";
    return 0;
  }

  hsize_t N=0;
  space.getSimpleExtentDims(&N);
  value.resize(N);
  attr.read(H5::PredType::NATIVE_DOUBLE, value.data());

  return true;
}

bool
Session::setAttribute(H5::DataSet &dataset, const std::string &name, const Eigen::Ref<const Eigen::VectorXd> &value) {
  hsize_t dims[1] = { hsize_t(value.size()) };
  H5::DataSpace fspace(1, dims);
  H5::Attribute attr = dataset.createAttribute(name, H5::PredType::NATIVE_DOUBLE, fspace);
  attr.write(H5::PredType::NATIVE_DOUBLE, value.data());
  fspace.close(); attr.close();
  return true;
}

bool
Session::readArray(H5::DataSet &dataset, Eigen::VectorXd &value) {
  if (H5T_FLOAT != dataset.getTypeClass()) {
    logError() << "Cannot read vector: Unexpected data type: " << dataset.getTypeClass()
               << ", expected " << H5T_FLOAT << ".";
    return 0;
  }

  H5::DataSpace dataspace = dataset.getSpace();
  if (1 != dataspace.getSimpleExtentNdims()) {
    logError() << "Cannot read vector: Unexpected rank-"
               << dataspace.getSimpleExtentNdims() << ", Expected rank-1.";
    return 0;
  }
  hsize_t N;
  dataspace.getSimpleExtentDims(&N, 0);

  value.resize(N);
  dataset.read(value.data(), H5::PredType::NATIVE_DOUBLE);

  return true;
}

bool
Session::readArray(H5::DataSet &dataset, Eigen::MatrixXcd &value) {
  H5::CompType ctype(sizeof(std::complex<double>));
  ctype.insertMember("re", 0, H5::PredType::NATIVE_DOUBLE);
  ctype.insertMember("im", sizeof(double), H5::PredType::NATIVE_DOUBLE);

  H5::DataSpace dataspace = dataset.getSpace();
  if (2 != dataspace.getSimpleExtentNdims()) {
    logError() << "Cannot read matrix: Unexpected rank-"
               << dataspace.getSimpleExtentNdims() << ", Expected rank-2.";
    return false;
  }

  hsize_t N[2];
  dataspace.getSimpleExtentDims(N, 0);

  value.resize(N[1], N[0]);
  try { dataset.read(value.data(), ctype); }
  catch (H5::DataSetIException error) {
    logError() << "Cannot read matrix: " << error.getCDetailMsg();
    return false;
  }

  return true;
}
