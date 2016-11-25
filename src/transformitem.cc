#include "transformitem.hh"
#include "timeseriesitem.hh"
#include <QVBoxLayout>
#include <QLabel>


/* ******************************************************************************************** *
 * Implementation of TransformTask
 * ******************************************************************************************** */
TransformTask::TransformTask(wt::Wavelet &wavelet, const Eigen::Ref<const Eigen::VectorXcd> &timeseries,
    const Eigen::Ref<const Eigen::VectorXd> &scales, Eigen::Ref<Eigen::MatrixXcd> result,
    QObject *parent)
  : QThread(parent), _wavelet(wavelet), _timeseries(timeseries), _scales(scales), _result(result),
    _trafo(0)
{
  // pass...
}

TransformTask::~TransformTask() {
  if (_trafo)
    delete _trafo;
}

void
TransformTask::run() {
  logDebug() << "Start wavelet transform...";
  _trafo = new wt::WaveletTransform(_wavelet, _scales);
  wt::ProgressDelegate<TransformTask> delegate(*this, &TransformTask::progresscb);
  (*_trafo)(_timeseries, _result, &delegate);
  logDebug() << "  ... done.";
}

void
TransformTask::progresscb(double frac) {
  emit progress(100*frac);
}


/* ******************************************************************************************** *
 * Implementation of TransformItem
 * ******************************************************************************************** */
TransformItem::TransformItem(TimeseriesItem *timeseries, wt::Wavelet &wavelet,
                             const Eigen::Ref<const Eigen::VectorXd> &scales,
                             TransformedItem::Scaling scaling, const QString &label, QObject *parent)
  : Item(label, parent), _timeseries(timeseries->size()), _scales(scales), _scaling(scaling),
    _result(_timeseries.size(), scales.size()), _wavelet(wavelet),
    _task(_wavelet, _timeseries, _scales, _result),
    _Fs(timeseries->Fs()), _t0(timeseries->t0())
{
  _icon  = QIcon("://icons/task16.png");

  // copy TS values
  if (RealTimeseriesItem *ritem = dynamic_cast<RealTimeseriesItem *>(timeseries)) {
    for (int i=0; i<_timeseries.size(); i++) {
      _timeseries(i) = ritem->data()(i);
    }
  } else if (ComplexTimeseriesItem *citem = dynamic_cast<ComplexTimeseriesItem *>(timeseries)) {
    for (int i=0; i<_timeseries.size(); i++) {
      _timeseries(i) = citem->data()(i);
    }
  }
  connect(&_task, SIGNAL(started()), this, SLOT(onTaskStarted()));
  connect(&_task, SIGNAL(progress(int)), this, SIGNAL(progress(int)));
  connect(&_task, SIGNAL(finished()), this, SLOT(onTaskFinished()));
}

void
TransformItem::start() {
  _task.start();
}

void
TransformItem::terminate() {
  _task.terminate();
}

double
TransformItem::Fs() const {
  return _Fs;
}

double
TransformItem::t0() const {
  return _t0;
}

const Eigen::MatrixXcd &
TransformItem::result() const {
  return _result;
}

const Eigen::VectorXd &
TransformItem::scales() const {
  return _scales;
}

TransformedItem::Scaling
TransformItem::scaling() const {
  return _scaling;
}

const wt::Wavelet &
TransformItem::wavelet() const {
  return _wavelet;
}

QWidget *
TransformItem::view() {
  return new TransformItemView(this);
}

void
TransformItem::onTaskStarted() {
  emit started(this);
}

void
TransformItem::onTaskFinished() {
  emit finished(this);
}


/* ********************************************************************************************* *
 * Implementation of TransformItemView
 * ********************************************************************************************* */
TransformItemView::TransformItemView(TransformItem *item, QWidget *parent)
  : QWidget(parent), _item(item)
{
  _progress = new QProgressBar();
  _progress->setMinimum(0);
  _progress->setMaximum(100);

  QVBoxLayout *layout = new QVBoxLayout();
  QLabel *label = new QLabel(tr("Perfrom wavelet transform ..."));
  QFont font = label->font(); font.setPointSize(28); label->setFont(font);
  layout->addWidget(new QWidget(),1);
  layout->addWidget(label);
  layout->addWidget(_progress);
  layout->addWidget(new QWidget(),1);
  setLayout(layout);

  connect(item, SIGNAL(progress(int)), _progress, SLOT(setValue(int)));
  connect(item, SIGNAL(destroyed(QObject*)), this, SLOT(deleteLater()));
}
