#include "application.hh"
#include <QFileDialog>
#include <QInputDialog>
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


Application::Application(int &argc, char **argv)
  : QApplication(argc, argv), _items(0), _procStatTimer(), _procStat()
{
  _items = new ItemModel(this);

  _procStatTimer.setInterval(10000);
  _procStatTimer.setSingleShot(false);
  connect(&_procStatTimer, SIGNAL(timeout()), &_procStat, SLOT(update()));
  connect(&_procStat, SIGNAL(updated(double,double)), this, SIGNAL(procStats(double,double)));
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

  int column = 0;
  if (1 < data.cols()) {
    bool ok;
    column = QInputDialog::getInt(
          0, tr("Select column"),
          tr("The file contains more than one column, select the column to import."),
          0, 0, data.cols(), 1, &ok);
    if (! ok)
      return false;
  }
  logDebug() << "CSV: read column " << column;

  bool ok;
  double Fs = QInputDialog::getDouble(
        0, tr("Set sample rate"), tr("Specify the sample rate for the time series."),
        1, 0, 1e12, 3, &ok);
  if (! ok)
    return false;

  QFileInfo finfo(filename);
  QString name = finfo.baseName();
  return addTimeseries(name, data.col(column), Fs);
}

bool
Application::addTimeseries(const QString &label, const Eigen::Ref<const Eigen::VectorXd> &data, double Fs) {
  _items->addItem(new TimeseriesItem(data, Fs, label));
  return true;
}

bool
Application::startTransform(TimeseriesItem *item) {
  TransformDialog dialog(item->data().size(), item->Fs());
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
  }

  TransformItem *titem = new TransformItem(item, wavelet, scales, dialog.scaling(), this);
  _items->addItem(titem);
  connect(titem, SIGNAL(finished(TransformItem*)), this, SLOT(onTransformFinished(TransformItem*)));
  titem->start();

  return true;
}


bool
Application::startSynthesis(TransformedItem *item) {
  SynthesisItem *sitem = new SynthesisItem(item, this);
  _items->addItem(sitem);
  connect(sitem, SIGNAL(finished(SynthesisItem*)), this, SLOT(onSynthesisFinished(SynthesisItem*)));
  sitem->start();
  return true;
}

bool
Application::startProjection(TransformedItem *item) {
  ProjectionItem *pitem = new ProjectionItem(item, this);
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
  TimeseriesItem *sitem = new TimeseriesItem(item->result().real(), item->Fs(), item->label());
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
