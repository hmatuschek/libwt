#include "synthesisitem.hh"
#include "transformeditem.hh"
#include <QVBoxLayout>
#include <QLabel>


/* ******************************************************************************************** *
 * Implementation of SynthesisTask
 * ******************************************************************************************** */
SynthesisTask::SynthesisTask(wt::Wavelet &wavelet, const Eigen::Ref<const Eigen::MatrixXcd> &transformed,
    const Eigen::Ref<const Eigen::VectorXd> &scales, Eigen::Ref<Eigen::VectorXcd> result,
    QObject *parent)
  : QThread(parent), _wavelet(wavelet), _scales(scales), _transformed(transformed), _result(result),
    _synthesis(0)
{
  // pass...
}

SynthesisTask::~SynthesisTask() {
  if (_synthesis)
    delete _synthesis;
}

void
SynthesisTask::run() {
  logDebug() << "Start wavelet synthesis ...";
  _synthesis = new wt::WaveletSynthesis(_wavelet, _scales);
  wt::ProgressDelegate<SynthesisTask> delegate(*this, &SynthesisTask::progresscb);
  (*_synthesis)(_transformed, _result, &delegate);
  logDebug() << "  ... done.";
}

void
SynthesisTask::progresscb(double frac) {
  emit progress(100*frac);
}


/* ******************************************************************************************** *
 * Implementation of SynthesisItem
 * ******************************************************************************************** */
SynthesisItem::SynthesisItem(TransformedItem *transformed, QObject *parent)
  : Item(parent), _transformed(transformed->data()), _scales(transformed->scales()),
    _result(_transformed.rows()), _wavelet(transformed->wavelet()),
    _task(_wavelet, _transformed, _scales, _result),
    _Fs(transformed->Fs())
{
  _label = transformed->label();
  _icon  = QIcon("://icons/task16.png");

  connect(&_task, SIGNAL(started()), this, SLOT(onTaskStarted()));
  connect(&_task, SIGNAL(progress(int)), this, SIGNAL(progress(int)));
  connect(&_task, SIGNAL(finished()), this, SLOT(onTaskFinished()));
}

void
SynthesisItem::start() {
  _task.start();
}

void
SynthesisItem::terminate() {
  _task.terminate();
}

double
SynthesisItem::Fs() const {
  return _Fs;
}

const Eigen::VectorXcd &
SynthesisItem::result() const {
  return _result;
}

const Eigen::VectorXd &
SynthesisItem::scales() const {
  return _scales;
}

const wt::Wavelet &
SynthesisItem::wavelet() const {
  return _wavelet;
}

QWidget *
SynthesisItem::view() {
  return new SynthesisItemView(this);
}

void
SynthesisItem::onTaskStarted() {
  emit started(this);
}

void
SynthesisItem::onTaskFinished() {
  emit finished(this);
}


/* ********************************************************************************************* *
 * Implementation of SynthesisItemView
 * ********************************************************************************************* */
SynthesisItemView::SynthesisItemView(SynthesisItem *item, QWidget *parent)
  : QWidget(parent), _item(item)
{
  _progress = new QProgressBar();
  _progress->setMinimum(0);
  _progress->setMaximum(100);

  QVBoxLayout *layout = new QVBoxLayout();
  QLabel *label = new QLabel(tr("Perfrom wavelet synthesis ..."));
  QFont font = label->font(); font.setPointSize(28); label->setFont(font);
  layout->addWidget(new QWidget(),1);
  layout->addWidget(label);
  layout->addWidget(_progress);
  layout->addWidget(new QWidget(),1);
  setLayout(layout);

  connect(item, SIGNAL(progress(int)), _progress, SLOT(setValue(int)));
  connect(item, SIGNAL(destroyed(QObject*)), this, SLOT(deleteLater()));
}


