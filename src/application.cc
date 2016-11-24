#include "application.hh"
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include "api.hh"
#include "utils/csv.hh"
#include "utils/logger.hh"
#include <fstream>
#include "transformdialog.hh"
#include "item.hh"
#include "timeseriesitem.hh"
#include "transformeditem.hh"
#include "transformitem.hh"
#include "synthesisitem.hh"
#include "projectionitem.hh"
#include "session.hh"
#include "importdialog.hh"


Application::Application(int &argc, char **argv)
  : QApplication(argc, argv), _items(0), _procStatTimer(), _procStat()
{
  _items = new ItemModel(this);

  _procStatTimer.setInterval(10000);
  _procStatTimer.setSingleShot(false);
  connect(&_procStatTimer, SIGNAL(timeout()), &_procStat, SLOT(update()));
  connect(&_procStat, SIGNAL(updated(double,double,double)),
          this, SIGNAL(procStats(double,double,double)));
  _procStatTimer.start();
}

Application::~Application() {
  // pass...
}

bool
Application::importTimeseries() {
  QString filename = QFileDialog::getOpenFileName(
        0, tr("Import timeseries"), "", tr("CSV (*.txt *.csv)"));
  if (! filename.size())
    return false;
  return importTimeseries(filename);
}

bool
Application::importTimeseries(const QString &filename) {
  std::ifstream file;
  file.open(filename.toUtf8().constData());
  if(! file.is_open()) {
    logError() << "Cannot read data from '" << filename.toUtf8().constData()
               << "': Cannot open file for reading.";
    return false;
  }

  Eigen::MatrixXd data;
  wt::CSV::read(data, file);
  file.close();

  if ((0 == data.rows()) || (0 == data.cols())) {
    logError() << "Cannot read data from '" << filename.toUtf8().constData()
               << "': Malformed CSV file.";
    return false;
  }

  ImportDialog dialog(filename, data.cols(), *this);
  if (QDialog::Accepted != dialog.exec())
    return false;

  if (dialog.real()) {
    return addTimeseries(
          dialog.label(), Eigen::Ref<const Eigen::VectorXd>(data.col(dialog.realColumn())), dialog.Fs());
  }

  Eigen::VectorXcd tmp(data.rows());
  for (int i=0; i<data.rows(); i++) {
    tmp(i) = std::complex<double>(data(i, dialog.realColumn()),
                                  data(i, dialog.imagColumn()));
  }

  return addTimeseries(dialog.label(), Eigen::Ref<const Eigen::VectorXcd>(tmp), dialog.Fs());
}

bool
Application::addTimeseries(const QString &label, const Eigen::Ref<const Eigen::VectorXd> &data, double Fs) {
  _items->addItem(new RealTimeseriesItem(data, Fs, label));
  return true;
}

bool
Application::addTimeseries(const QString &label, const Eigen::Ref<const Eigen::VectorXcd> &data, double Fs) {
  _items->addItem(new ComplexTimeseriesItem(data, Fs, label));
  return true;
}

bool
Application::startTransform(TimeseriesItem *item) {
  TransformDialog dialog(item->size(), item->Fs());
  if (QDialog::Rejected == dialog.exec())
    return false;

  Eigen::VectorXd scales(dialog.numScales());
  double minScale = std::min(dialog.minScale(),dialog.maxScale());
  double maxScale = std::max(dialog.minScale(),dialog.maxScale());

  switch (dialog.scaling()) {
  case TransformedItem::LINEAR:
    wt::linear_range(minScale*item->Fs(), maxScale*item->Fs(), scales);
    break;
  case TransformedItem::DYADIC:
    wt::dyadic_range(minScale*item->Fs(), maxScale*item->Fs(), scales);
    break;
  case TransformedItem::DECADIC:
    wt::decadic_range(minScale*item->Fs(), maxScale*item->Fs(), scales);
    break;
  }

  wt::Wavelet wavelet;
  switch (dialog.wavelet()) {
  case TransformDialog::MORLET_WAVELET:
    wavelet = wt::Morlet(dialog.param1());
    break;
  case TransformDialog::CAUCHY_WAVELET:
    wavelet = wt::Cauchy(dialog.param1());
    break;
  case TransformDialog::REGMORLET_WAVELET:
    wavelet = wt::RegMorlet(dialog.param1());
    break;
  }

  QString label = tr("W:%0").arg(item->label());
  while (_items->contains(label) || label.isEmpty()) {
    bool ok;
    label = QInputDialog::getText(
          0, tr("Choose a label"),
          tr("There is already an item labeled '%0', choose a different label.").arg(label),
          QLineEdit::Normal, label, &ok);
    if (! ok)
      return false;
  }

  TransformItem *titem = new TransformItem(item, wavelet, scales, dialog.scaling(), label, this);
  _items->addItem(titem);
  connect(titem, SIGNAL(finished(TransformItem*)), this, SLOT(onTransformFinished(TransformItem*)));
  titem->start();

  return true;
}


bool
Application::startSynthesis(TransformedItem *item)
{
  QString label = tr("M:%0").arg(item->label());
  while (_items->contains(label) || label.isEmpty()) {
    bool ok;
    label = QInputDialog::getText(
          0, tr("Choose a label"),
          tr("There is already an item labeled '%0', choose a different label.").arg(label),
          QLineEdit::Normal, label, &ok);
    if (! ok)
      return false;
  }

  SynthesisItem *sitem = new SynthesisItem(item, label, this);
  _items->addItem(sitem);
  connect(sitem, SIGNAL(finished(SynthesisItem*)), this, SLOT(onSynthesisFinished(SynthesisItem*)));
  sitem->start();
  return true;
}

bool
Application::startProjection(TransformedItem *item)
{
  QString label = tr("P:%0").arg(item->label());
  while (_items->contains(label) || label.isEmpty()) {
    bool ok;
    label = QInputDialog::getText(
          0, tr("Choose a label"),
          tr("There is already an item labeled '%0', choose a different label.").arg(label),
          QLineEdit::Normal, label, &ok);
    if (! ok)
      return false;
  }

  ProjectionItem *pitem = new ProjectionItem(item, label, this);
  _items->addItem(pitem);
  connect(pitem, SIGNAL(finished(ProjectionItem*)), this, SLOT(onProjectionFinished(ProjectionItem*)));
  pitem->start();
  return true;
}

void
Application::onTransformFinished(TransformItem *item) {
  TransformedItem *ritem = new TransformedItem(
        item->wavelet(), item->Fs(), item->scales(), item->scaling(), item->result(),item->label());
  _items->addItem(ritem);
  _items->remItem(item);
}

void
Application::onSynthesisFinished(SynthesisItem *item) {
  ComplexTimeseriesItem *sitem = new ComplexTimeseriesItem(item->result(), item->Fs(), item->label());
  _items->addItem(sitem);
  _items->remItem(item);
}

void
Application::onProjectionFinished(ProjectionItem *item) {
  TransformedItem *pitem = new TransformedItem(item->wavelet(), item->Fs(), item->scales(),
                                               item->scaling(), item->result(), item->label());
  _items->addItem(pitem);
  _items->remItem(item);
}

bool
Application::saveSession() {
  QString filename = QFileDialog::getSaveFileName(
        0, tr("Save session"), "", tr("HDF5 files (*.h5 *.hdf5)"), 0);

  if (filename.isEmpty())
    return false;

  bool success = Session::save(filename, this);

  if (! success) {
    QMessageBox::critical(
          0, tr("Cannot save session."),
          tr("Cannot save session as %0.").arg(filename));
  }

  return success;
}

bool
Application::loadSession() {
  QString filename = QFileDialog::getOpenFileName(
        0, tr("Save session"), "", tr("HDF5 files (*.h5 *.hdf5)"));

  if (filename.isEmpty())
    return false;

  return addSession(filename);
}

bool
Application::addSession() {
  QString filename = QFileDialog::getOpenFileName(
        0, tr("Save session"), "", tr("HDF5 files (*.h5 *.hdf5)"));

  if (filename.isEmpty())
    return false;

  return addSession(filename);
}

bool
Application::addSession(const QString &filename) {
  bool success = Session::load(filename, this);
  if (! success) {
    QMessageBox::critical(
          0, tr("Cannot load session."),
          tr("Cannot load session from %0.").arg(filename));
  }
  return success;
}
