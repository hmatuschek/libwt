#ifndef ITEM_HH
#define ITEM_HH

#include <QObject>
#include <QAbstractListModel>
#include <QIcon>

#include <Eigen/Eigen>
#include "api.hh"


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
  TimeseriesItem(const Eigen::Ref<const Eigen::VectorXd> &data, double Fs, QObject *parent=0);
  virtual ~TimeseriesItem();

  inline double Fs() const { return _Fs; }
  inline const Eigen::VectorXd &data() const { return _data; }

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


class ItemModel: public QAbstractListModel
{
  Q_OBJECT

public:
  ItemModel(QObject *parent=0);
  virtual ~ItemModel();

  void addItem(Item *item);
  void remItem(size_t i);
  Item *item(size_t i);

  int rowCount(const QModelIndex &parent) const;
  QVariant data(const QModelIndex &index, int role) const;

protected slots:
  void _onItemUpdated(Item *item);

protected:
  QVector<Item *> _items;
};

#endif // ITEM_HH
