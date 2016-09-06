#ifndef TRANSFORMITEM_HH
#define TRANSFORMITEM_HH

#include <QThread>
#include "item.hh"
#include <QProgressBar>
#include "api.hh"
#include "wavelettransform.hh"
#include "transformeditem.hh"

class TimeseriesItem;


class TransformTask: public QThread
{
  Q_OBJECT

public:
  TransformTask(wt::Wavelet &wavelet,
                const Eigen::Ref<const Eigen::VectorXcd> &timeseries,
                const Eigen::Ref<const Eigen::VectorXd> &scales,
                Eigen::Ref<Eigen::MatrixXcd> result,
                QObject *parent=0);

  virtual ~TransformTask();

signals:
  void progress(int);

protected:
  virtual void run();
  void progresscb(double frac);

protected:
  wt::Wavelet &_wavelet;
  Eigen::Ref<const Eigen::VectorXcd> _timeseries;
  Eigen::Ref<const Eigen::VectorXd> _scales;
  Eigen::Ref<Eigen::MatrixXcd> _result;
  wt::WaveletTransform *_trafo;
};


class TransformItem: public Item
{
  Q_OBJECT

public:
  TransformItem(TimeseriesItem *timeseries, wt::Wavelet &wavelet,
                const Eigen::Ref<const Eigen::VectorXd> &scales,
                TransformedItem::Scaling scaling,
                const QString &label="transformed",
                QObject *parent=0);

  double Fs() const;
  const Eigen::MatrixXcd &result() const;
  const Eigen::VectorXd &scales() const;
  TransformedItem::Scaling scaling() const;
  const wt::Wavelet &wavelet() const;

  QWidget *view();

public slots:
  void start();
  void terminate();

signals:
  void started(TransformItem *item);
  void progress(int);
  void finished(TransformItem *item);


protected slots:
  void onTaskStarted();
  void onTaskFinished();

protected:
  Eigen::VectorXcd _timeseries;
  Eigen::VectorXd _scales;
  TransformedItem::Scaling _scaling;
  Eigen::MatrixXcd _result;
  wt::Wavelet _wavelet;
  TransformTask _task;
  double _Fs;
};


class TransformItemView: public QWidget
{
  Q_OBJECT

public:
  TransformItemView(TransformItem *item, QWidget *parent=0);

protected:
  TransformItem *_item;
  QProgressBar *_progress;
};


#endif // TRANSFORMITEM_HH
