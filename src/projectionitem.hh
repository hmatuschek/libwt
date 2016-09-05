#ifndef PROJECTIONITEM_HH
#define PROJECTIONITEM_HH

#include <QThread>
#include <QProgressBar>

#include "item.hh"
#include "api.hh"
#include "waveletconvolution.hh"
#include "transformeditem.hh"


class ProjectionTask: public QThread
{
  Q_OBJECT

public:
  ProjectionTask(const wt::Wavelet &wavelet,
                const Eigen::Ref<const Eigen::MatrixXcd> &transformed,
                const Eigen::Ref<const Eigen::VectorXd> &scales,
                Eigen::Ref<Eigen::MatrixXcd> result,
                QObject *parent=0);

  virtual ~ProjectionTask();

signals:
  void progress(int);

protected:
  virtual void run();
  void progresscb(double frac);

protected:
  wt::Wavelet _wavelet;
  Eigen::Ref<const Eigen::VectorXd> _scales;
  Eigen::Ref<Eigen::MatrixXcd> _result;
  Eigen::Ref<const Eigen::MatrixXcd> _transformed;
  wt::WaveletConvolution *_projection;
};


class ProjectionItem: public Item
{
  Q_OBJECT

public:
  ProjectionItem(TransformedItem *transformed, QObject *parent=0);

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
  void started(ProjectionItem *item);
  void progress(int);
  void finished(ProjectionItem *item);


protected slots:
  void onTaskStarted();
  void onTaskFinished();

protected:
  Eigen::MatrixXcd _transformed;
  Eigen::VectorXd _scales;
  TransformedItem::Scaling _scaling;
  Eigen::MatrixXcd _result;
  wt::Wavelet _wavelet;
  ProjectionTask _task;
  double _Fs;
};


class ProjectionItemView: public QWidget
{
  Q_OBJECT

public:
  ProjectionItemView(ProjectionItem *item, QWidget *parent=0);

protected:
  ProjectionItem *_item;
  QProgressBar *_progress;
};

#endif // PROJECTIONITEM_HH
