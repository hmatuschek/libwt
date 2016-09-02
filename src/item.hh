#ifndef ITEM_HH
#define ITEM_HH

#include <QObject>
#include <QAbstractListModel>
#include <QIcon>
#include <QThread>

#include <Eigen/Eigen>
#include "api.hh"
#include "wavelettransform.hh"


/** Base class of all items held by the application. This includes time series, wavelet transforms
 * etc. */
class Item : public QObject
{
  Q_OBJECT

protected:
  /** Hidden constructor. */
  explicit Item(QObject *parent = 0);

public:
  virtual ~Item();

  virtual QWidget *view() = 0;

  const QString &label() const;
  void setLabel(const QString &label);
  const QIcon &icon() const;
  void setIcon(const QIcon &icon);

signals:
  void updated(Item *item);

protected:
  QString _label;
  QIcon _icon;
};


class TimeseriesItem: public Item
{
  Q_OBJECT

public:
  TimeseriesItem(const Eigen::Ref<const Eigen::VectorXd> &data, double Fs, const QString &label="timeseries", QObject *parent=0);
  virtual ~TimeseriesItem();

  inline double Fs() const { return _Fs; }
  inline const Eigen::VectorXd &data() const { return _data; }

  QWidget *view();

protected:
  Eigen::VectorXd _data;
  double _Fs;
};


class WaveletTransformedItem: public Item
{
  Q_OBJECT

public:
  WaveletTransformedItem(const wt::Wavelet &wavelet, double Fs, const Eigen::Ref<const Eigen::VectorXd> &scales,
                         const Eigen::Ref<const Eigen::MatrixXcd> &data, QObject *parent=0);
  virtual ~WaveletTransformedItem();

  inline const wt::Wavelet &wavelet() const { return _wavelet; }
  inline double Fs() const { return _Fs; }
  inline const Eigen::VectorXd &scales() const { return _scales; }
  inline const Eigen::MatrixXcd &data() const { return _data; }

protected:
  wt::Wavelet _wavelet;
  double _Fs;
  Eigen::VectorXd _scales;
  Eigen::MatrixXcd _data;
};


class TransformTask: public QThread
{
  Q_OBJECT

public:
  TransformTask(wt::Wavelet &wavelet,
                const Eigen::Ref<const Eigen::VectorXcd> &timeseries,
                const Eigen::Ref<const Eigen::VectorXd> &scales,
                Eigen::Ref<Eigen::MatrixXcd> result, QObject *parent=0);

protected:
  virtual void run();

protected:
  Eigen::Ref<Eigen::MatrixXcd> _result;
  Eigen::Ref<const Eigen::VectorXcd> _timeseries;
  wt::WaveletTransform _trafo;
};


class TransformItem: public Item
{
  Q_OBJECT

public:
  TransformItem(TimeseriesItem *timeseries, wt::Wavelet &wavelet,
                const Eigen::Ref<const Eigen::VectorXd> &scales, QObject *parent);

  const QString &label() const;
  double Fs() const;
  const Eigen::MatrixXcd &result() const;

public slots:
  void start();
  void terminate();

signals:
  void started(TransformItem *item);
  void finished(TransformItem *item);

protected slots:
  void onTaskStarted();
  void onTaskFinished();

protected:
  Eigen::VectorXcd _timeseries;
  Eigen::MatrixXcd _result;
  TransformTask _task;
  QString _label;
  double _Fs;
};


class ItemModel: public QAbstractListModel
{
  Q_OBJECT

public:
  ItemModel(QObject *parent=0);
  virtual ~ItemModel();

  void addItem(Item *item);
  void remItem(size_t i);
  void remItem(Item *item);
  Item *item(size_t i);

  int rowCount(const QModelIndex &parent) const;
  QVariant data(const QModelIndex &index, int role) const;

protected slots:
  void _onItemUpdated(Item *item);

protected:
  QVector<Item *> _items;
};

#endif // ITEM_HH
