#include "item.hh"
#include <QWidget>
#include "itemview.hh"


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
TimeseriesItem::TimeseriesItem(const Eigen::Ref<const Eigen::VectorXd> &data, double Fs,
                               const QString &label, QObject *parent)
  : Item(parent), _data(data), _Fs(Fs)
{
  _label = label;
  _icon  = QIcon();
}

TimeseriesItem::~TimeseriesItem() {
  // pass...
}

QWidget *
TimeseriesItem::view() {
  QWidget *view = new TimeseriesItemView(this);
  connect(this, SIGNAL(destroyed()), view, SLOT(deleteLater()));
  return view;
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
 * Implementation of TransformTask
 * ******************************************************************************************** */
TransformTask::TransformTask(wt::Wavelet &wavelet, const Eigen::Ref<const Eigen::VectorXcd> &timeseries,
    const Eigen::Ref<const Eigen::VectorXd> &scales, Eigen::Ref<Eigen::MatrixXcd> result,
    QObject *parent)
  : QThread(parent), _result(result), _timeseries(timeseries), _trafo(wavelet, scales)
{
  // pass...
}

void
TransformTask::run() {
  _trafo(_timeseries, _result);
}


/* ******************************************************************************************** *
 * Implementation of TransformItem
 * ******************************************************************************************** */
TransformItem::TransformItem(TimeseriesItem *timeseries, wt::Wavelet &wavelet,
                             const Eigen::Ref<const Eigen::VectorXd> &scales, QObject *parent)
  : Item(parent), _timeseries(timeseries->data().size()),
    _result(_timeseries.size(), scales.size()),
    _task(wavelet, _timeseries, scales, _result),
    _label(timeseries->label()), _Fs(timeseries->Fs())
{
  // copy TS values
  for (int i=0; i<_timeseries.size(); i++) {
    _timeseries(i) = timeseries->data()(i);
  }
  connect(&_task, SIGNAL(started()), this, SLOT(onTaskStarted()));
  connect(&_task, SIGNAL(finished()), this, SLOT(onTaskFinished()));
}

void
TransformItem::start() {
  _task.start();
}

void
TransformItem::terminate() {
  _task.terminate();
}

void
TransformItem::onTaskStarted() {
  emit started(this);
}

void
TransformItem::onTaskFinished() {
  emit finished(this);
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
  }
  return QVariant();
}
