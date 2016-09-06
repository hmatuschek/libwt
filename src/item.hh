#ifndef ITEM_HH
#define ITEM_HH

#include <QObject>
#include <QAbstractListModel>
#include <QIcon>


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


class ItemModel: public QAbstractListModel
{
  Q_OBJECT

public:
  ItemModel(QObject *parent=0);
  virtual ~ItemModel();

  bool contains(const QString &label) const;
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
