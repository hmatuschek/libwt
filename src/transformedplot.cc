#include "transformedplot.hh"
#include "transformeditem.hh"
#include "fmtutil.hh"
#include <iostream>


/* ********************************************************************************************* *
 * Implementation of TransformedPlot::Settings
 * ********************************************************************************************* */
TransformedPlot::Settings::Settings()
  : _showTitle(true)
{
  // pass...
}

TransformedPlot::Settings::Settings(const Settings &other)
  : _showTitle(other._showTitle)
{
  // pass...
}

TransformedPlot::Settings &
TransformedPlot::Settings::operator =(const Settings &other) {
  _showTitle = other._showTitle;
  return *this;
}

bool
TransformedPlot::Settings::showTitle() const {
  return _showTitle;
}

void
TransformedPlot::Settings::setShowTitle(bool show) {
  _showTitle = show;
}


/* ********************************************************************************************* *
 * Implementation of TransformedPlot
 * ********************************************************************************************* */
TransformedPlot::TransformedPlot(TransformedItem *item, const Settings &settings, QWidget *parent)
  : QCustomPlot(parent), _item(item), _settings(settings), _title(0),
    _cropping(false), _curve(0), _polygon()
{
  axisRect()->setupFullAxesBox(true);
  plotLayout()->insertRow(0);
  QString title = tr("Wavelet transformed '%0' [Fs=%1]")
      .arg(_item->label(), fmt_freq(_item->Fs()));
  _title = new QCPPlotTitle(this, title);
  plotLayout()->addElement(0, 0, _title);
  _title->setVisible(_settings.showTitle());
  xAxis->setLabel("Time [s]");
  yAxis->setLabel("Scale [s]");

  QCPColorMap *colorMap = new QCPColorMap(xAxis, yAxis);
  addPlottable(colorMap);
  colorMap->data()->setSize(item->data().rows(), item->data().cols());
  colorMap->data()->setRange(QCPRange(0, item->data().rows()/item->Fs()),
                             QCPRange(item->scales()(0), item->scales()(item->scales().size()-1)));
  for (int i=0; i<item->data().rows(); i++) {
    for (int j=0; j<item->data().cols(); j++)
      colorMap->data()->setCell(i, j, std::abs(item->data()(i,j)));
  }

  switch (_item->scaling()) {
    case TransformedItem::LINEAR:
      break;
    case TransformedItem::DYADIC:
      yAxis->setScaleType(QCPAxis::stLogarithmic);
      yAxis->setScaleLogBase(2);
      break;
    case TransformedItem::DECADIC:
      yAxis->setScaleType(QCPAxis::stLogarithmic);
      yAxis->setScaleLogBase(10);
      break;
  }

  QCPColorScale *colorScale = new QCPColorScale(this);
  plotLayout()->addElement(1, 1, colorScale);
  colorScale->setType(QCPAxis::atRight);
  colorMap->setColorScale(colorScale);
  colorScale->axis()->setLabel("Modulus [a.u.]");

  colorMap->setGradient(QCPColorGradient::gpHot);
  colorMap->rescaleDataRange();

  QCPMarginGroup *marginGroup = new QCPMarginGroup(this);
  axisRect()->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);
  colorScale->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);
  rescaleAxes();

  _valid = new QCPCurve(xAxis, yAxis);
  addPlottable(_valid);
  QPen pen = _valid->pen(); pen.setColor(Qt::black); _valid->setPen(pen);
  _valid->setVisible(true);
  for (int j=0; j<_item->scales().size(); j++) {
    double w = _item->wavelet().cutOffTime()*_item->scales()(j)/2;
    if (w < xAxis->range().center())
      _valid->addData(w,_item->scales()(j));
  }
  for (int j=(_item->scales().size()-1); j>=0; j--) {
    double w = _item->wavelet().cutOffTime()*_item->scales()(j)/2;
    if (w < xAxis->range().center())
      _valid->addData(xAxis->range().upper-w, _item->scales()(j));
  }

  _curve = new QCPCurve(xAxis, yAxis);
  addPlottable(_curve);
  pen = _curve->pen(); pen.setWidth(2); _curve->setPen(pen);
  _curve->setVisible(false);

  applySettings(settings);
}

const TransformedPlot::Settings &
TransformedPlot::settings() const {
  return _settings;
}

void
TransformedPlot::applySettings(const Settings &settings) {
  _settings = settings;
  QString title = tr("Wavelet transformed '%0' [Fs=%1]")
      .arg(_item->label(), fmt_freq(_item->Fs()));
  _title->setText(title);
  _title->setVisible(_settings.showTitle());
  replot();
}

void
TransformedPlot::crop(bool crop) {
  _cropping = crop;
  if (_cropping) {
    _start = QPoint(-1,-1);
    _curve->clearData();
    _curve->setVisible(true);
    _polygon.clear();
  } else {
    _curve->setVisible(false);
    replot();
  }
}

void
TransformedPlot::mouseReleaseEvent(QMouseEvent *event) {
  QCustomPlot::mouseReleaseEvent(event);
  if (! _cropping)
    return;

  double x = xAxis->pixelToCoord(event->x());
  x = std::max(xAxis->range().lower, x);
  x = std::min(xAxis->range().upper, x);
  double y = yAxis->pixelToCoord(event->y());
  y = std::max(yAxis->range().lower, y);
  y = std::min(yAxis->range().upper, y);

  if (0 == _polygon.size()) {
    _start = event->pos();
    _curve->addData(x,y);
    _polygon.add(x,y);
  } else if ((_start-event->pos()).manhattanLength()<10) {
    std::cerr << "Done." << std::endl;
    _curve->addData(_polygon(0)(0), _polygon(0)(1));
    _polygon.add(_polygon(0)(0), _polygon(0)(1));
    emit cropped(_polygon);
    _cropping = false;
  } else {
    _curve->addData(x,y);
    _polygon.add(x,y);
  }

  replot();
}

