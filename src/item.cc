#include "item.hh"

/* ******************************************************************************************** *
 * Implementation of Item
 * ******************************************************************************************** */
Item::Item(QObject *parent)
  : QObject(parent), _label(""), _icon()
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
 * Implementation of TimeseriesItem
 * ******************************************************************************************** */
TimeseriesItem::TimeseriesItem(const Eigen::Ref<const Eigen::VectorXd> &data, double Fs, QObject *parent)
  : Item(parent), _data(data), _Fs(Fs)
{
  // pass...
}

TimeseriesItem::~TimeseriesItem() {
  // pass...
}


/* ******************************************************************************************** *
 * Implementation of WaveletTransformedItem
 * ******************************************************************************************** */
WaveletTransformedItem::WaveletTransformedItem(
    const wt::Wavelet &wavelet, double Fs, const Eigen::Ref<const Eigen::VectorXd> &scales,
    const Eigen::Ref<const Eigen::MatrixXcd> &data, QObject *parent)
  : Item(parent), _wavelet(wavelet), _Fs(Fs), _scales(scales), _data(data)
{
  // pass...
}

WaveletTransformedItem::~WaveletTransformedItem() {
  // pass...
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
  if (i >= _items.size())
    return;
  beginRemoveRows(QModelIndex(), i, i);
  delete _items[i];
  _items.remove(i);
  endRemoveRows();
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
  }
  return QVariant();
}
