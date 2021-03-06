#include "item.hh"
#include <QWidget>
#include "itemview.hh"
#include "timeseriesitem.hh"
#include "transformeditem.hh"
#include "transformitem.hh"
#include "synthesisitem.hh"
#include "projectionitem.hh"


/* ******************************************************************************************** *
 * Implementation of Item
 * ******************************************************************************************** */
Item::Item(const QString &label, QObject *parent)
  : QObject(parent), _label(label), _icon()
{
  // pass...
}

Item::~Item() {
  // pass...
}

const QString &
Item::label() const {
  return _label;
}

void
Item::setLabel(const QString &label) {
  _label = label;
  emit updated(this);
}

const QIcon &
Item::icon() const {
  return _icon;
}

void
Item::setIcon(const QIcon &icon) {
  _icon = icon;
  emit updated(this);
}


/* ******************************************************************************************** *
 * Implementation of ItemModel
 * ******************************************************************************************** */
ItemModel::ItemModel(QObject *parent)
  : QAbstractListModel(parent), _items()
{
  // pass...
}

ItemModel::~ItemModel() {
  // pass...
}

bool
ItemModel::contains(const QString &label) const {
  for (int i=0; i<_items.size(); i++) {
    if (label == _items[i]->label())
      return true;
  }
  return false;
}

void
ItemModel::addItem(Item *item) {
  beginInsertRows(QModelIndex(), _items.size(),_items.size());
  item->setParent(this);
  connect(item, SIGNAL(updated(Item*)), this, SLOT(_onItemUpdated(Item*)));
  _items.push_back(item);
  endInsertRows();
}

void
ItemModel::remItem(size_t i) {
  if (int(i) >= _items.size())
    return;
  beginRemoveRows(QModelIndex(), i, i);
  _items[i]->deleteLater();
  _items.remove(i);
  endRemoveRows();
}

void
ItemModel::remItem(Item *item) {
  int idx = _items.indexOf(item);
  if ((idx>=0) && (idx<_items.size())) {
    remItem(idx);
  }
}

Item *
ItemModel::item(size_t i) {
  return _items[i];
}

void
ItemModel::_onItemUpdated(Item *item) {
  int idx = _items.indexOf(item);
  if ((idx >= 0) && (idx < _items.size())) {
    emit dataChanged(index(idx), index(idx));
  }
}

int
ItemModel::rowCount(const QModelIndex &parent) const {
  return _items.size();
}

QVariant
ItemModel::data(const QModelIndex &index, int role) const {
  if (index.row() >= _items.size())
    return QVariant();
  if (Qt::DisplayRole == role) {
    return _items[index.row()]->label();
  } else if (Qt::DecorationRole == role) {
    return _items[index.row()]->icon();
  } else if (Qt::UserRole == role) {
    if (dynamic_cast<TimeseriesItem *>(_items[index.row()]))
      return 0;
    if (dynamic_cast<TransformedItem *>(_items[index.row()]))
      return 1;
    if (dynamic_cast<TransformItem *>(_items[index.row()]))
      return 2;
    if (dynamic_cast<SynthesisItem *>(_items[index.row()]))
      return 2;
    if (dynamic_cast<ProjectionItem *>(_items[index.row()]))
      return 2;
  }
  return QVariant();
}
