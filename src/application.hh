#ifndef APPLICATION_HH
#define APPLICATION_HH

#include <QApplication>
#include <Eigen/Eigen>
#include <QTimer>
#include "procinfo.hh"


class ItemModel;
class TimeseriesItem;
class TransformedItem;
class TransformItem;
class SynthesisItem;
class ProjectionItem;


class Application: public QApplication
{
  Q_OBJECT

public:
  Application(int &argc, char **argv);
  virtual ~Application();

  inline ItemModel *items() { return _items; }

public slots:
  bool importTimeseries();
  bool importTimeseries(const QString &filename);
  bool addTimeseries(const QString &label, const Eigen::Ref<const Eigen::VectorXd> &data,
                     double Fs, double t0);
  bool addTimeseries(const QString &label, const Eigen::Ref<const Eigen::VectorXcd> &data,
                     double Fs, double t0);
  bool startTransform(TimeseriesItem *item);
  bool startSynthesis(TransformedItem *item);
  bool startProjection(TransformedItem *item);

  bool saveSession();
  bool loadSession();
  bool addSession();
  bool addSession(const QString &filename);

signals:
  void procStats(double mem, double cpu, double t);

protected slots:
  void onTransformFinished(TransformItem *item);
  void onSynthesisFinished(SynthesisItem *item);
  void onProjectionFinished(ProjectionItem *item);

protected:
  ItemModel *_items;
  QTimer _procStatTimer;
  ProcInfo _procStat;
};

#endif // APPLICATION_HH
