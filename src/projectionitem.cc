#include "projectionitem.hh"
#include "transformeditem.hh"
#include <QVBoxLayout>
#include <QLabel>


/* ******************************************************************************************** *
 * Implementation of ProjectionTask
 * ******************************************************************************************** */
ProjectionTask::ProjectionTask(const wt::Wavelet &wavelet, const Eigen::Ref<const Eigen::MatrixXcd> &transformed,
    const Eigen::Ref<const Eigen::VectorXd> &scales, Eigen::Ref<Eigen::MatrixXcd> result,
    QObject *parent)
  : QThread(parent), _wavelet(wavelet), _scales(scales), _result(result), _transformed(transformed),
    _projection(0)
{
  // pass...
}

ProjectionTask::~ProjectionTask() {
  if (_projection)
    delete _projection;
}

void
ProjectionTask::run() {
  logDebug() << "Start wavelet projection ...";
  _projection = new wt::WaveletConvolution(_wavelet, _scales);
  wt::ProgressDelegate<ProjectionTask> delegate(*this, &ProjectionTask::progresscb);
  (*_projection)(_transformed, _result, &delegate);
  logDebug() << "  ... done.";
}

void
ProjectionTask::progresscb(double frac) {
  emit progress(100*frac);
}


/* ******************************************************************************************** *
 * Implementation of ProjectionItem
 * ******************************************************************************************** */
ProjectionItem::ProjectionItem(TransformedItem *transformed, const QString &label, QObject *parent)
  : Item(parent), _transformed(transformed->data()), _scales(transformed->scales()),
    _scaling(transformed->scaling()), _result(_transformed.rows(), _transformed.cols()),
    _wavelet(transformed->wavelet()), _task(_wavelet, _transformed, _scales, _result),
    _Fs(transformed->Fs()), _t0(transformed->t0())
{
  _label = label;
  _icon  = QIcon("://icons/task16.png");

  connect(&_task, SIGNAL(started()), this, SLOT(onTaskStarted()));
  connect(&_task, SIGNAL(progress(int)), this, SIGNAL(progress(int)));
  connect(&_task, SIGNAL(finished()), this, SLOT(onTaskFinished()));
}

void
ProjectionItem::start() {
  _task.start();
}

void
ProjectionItem::terminate() {
  _task.terminate();
}

double
ProjectionItem::Fs() const {
  return _Fs;
}

double
ProjectionItem::t0() const {
  return _t0;
}

const Eigen::MatrixXcd &
ProjectionItem::result() const {
  return _result;
}

const Eigen::VectorXd &
ProjectionItem::scales() const {
  return _scales;
}

TransformedItem::Scaling
ProjectionItem::scaling() const {
  return _scaling;
}

const wt::Wavelet &
ProjectionItem::wavelet() const {
  return _wavelet;
}

QWidget *
ProjectionItem::view() {
  return new ProjectionItemView(this);
}

void
ProjectionItem::onTaskStarted() {
  emit started(this);
}

void
ProjectionItem::onTaskFinished() {
  emit finished(this);
}


/* ********************************************************************************************* *
 * Implementation of ProjectionItemView
 * ********************************************************************************************* */
ProjectionItemView::ProjectionItemView(ProjectionItem *item, QWidget *parent)
  : QWidget(parent), _item(item)
{
  _progress = new QProgressBar();
  _progress->setMinimum(0);
  _progress->setMaximum(100);

  QVBoxLayout *layout = new QVBoxLayout();
  QLabel *label = new QLabel(tr("Perfrom wavelet projection ..."));
  QFont font = label->font(); font.setPointSize(28); label->setFont(font);
  layout->addWidget(new QWidget(),1);
  layout->addWidget(label);
  layout->addWidget(_progress);
  layout->addWidget(new QWidget(),1);
  setLayout(layout);

  connect(item, SIGNAL(progress(int)), _progress, SLOT(setValue(int)));
  connect(item, SIGNAL(destroyed(QObject*)), this, SLOT(deleteLater()));
}


