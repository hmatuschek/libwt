#ifndef SYNTHESISITEM_HH
#define SYNTHESISITEM_HH

#include <QThread>
#include "item.hh"
#include <QProgressBar>
#include "api.hh"
#include "waveletsynthesis.hh"


class TransformedItem;


class SynthesisTask: public QThread
{
  Q_OBJECT

public:
  SynthesisTask(wt::Wavelet &wavelet,
                const Eigen::Ref<const Eigen::MatrixXcd> &transformed,
                const Eigen::Ref<const Eigen::VectorXd> &scales,
                Eigen::Ref<Eigen::VectorXcd> result,
                QObject *parent=0);

  virtual ~SynthesisTask();

signals:
  void progress(int);

protected:
  virtual void run();
  void progresscb(double frac);

protected:
  wt::Wavelet _wavelet;
  Eigen::Ref<const Eigen::VectorXd> _scales;
  Eigen::Ref<const Eigen::MatrixXcd> _transformed;
  Eigen::Ref<Eigen::VectorXcd> _result;
  wt::WaveletSynthesis *_synthesis;
};


class SynthesisItem: public Item
{
  Q_OBJECT

public:
  SynthesisItem(TransformedItem *transformed, const QString &label="synthesized", QObject *parent=0);

  double Fs() const;
  const Eigen::VectorXcd &result() const;
  const Eigen::VectorXd &scales() const;
  const wt::Wavelet &wavelet() const;

  QWidget *view();

public slots:
  void start();
  void terminate();

signals:
  void started(SynthesisItem *item);
  void progress(int);
  void finished(SynthesisItem *item);


protected slots:
  void onTaskStarted();
  void onTaskFinished();

protected:
  Eigen::MatrixXcd _transformed;
  Eigen::VectorXd _scales;
  Eigen::VectorXcd _result;
  wt::Wavelet _wavelet;
  SynthesisTask _task;
  double _Fs;
};


class SynthesisItemView: public QWidget
{
  Q_OBJECT

public:
  SynthesisItemView(SynthesisItem *item, QWidget *parent=0);

protected:
  SynthesisItem *_item;
  QProgressBar *_progress;
};




#endif // SYNTHESISITEM_HH
