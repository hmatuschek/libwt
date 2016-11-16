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
  _title = new QCPTextElement(this, title);
  QFont font = _title->font(); font.setPointSize(16);
  _title->setFont(font);
  plotLayout()->addElement(0, 0, _title);
  _title->setVisible(_settings.showTitle());
  xAxis->setLabel("Time [s]");
  yAxis->setLabel("Scale [s]");

  QCPColorMap *colorMap = new QCPColorMap(xAxis, yAxis);
  colorMap->data()->setSize(item->data().rows(), item->data().cols());
  colorMap->data()->setRange(QCPRange(0, item->data().rows()/item->Fs()),
                             QCPRange(item->scales()(0), item->scales()(item->scales().size()-1)));
  for (int i=0; i<item->data().rows(); i++) {
    for (int j=0; j<item->data().cols(); j++)
      colorMap->data()->setCell(i, j, std::abs(item->data()(i,j)));
  }

  _rkOverlay = new QCPColorMap(xAxis, yAxis);
  _rkOverlay->data()->setSize(item->data().rows(), item->data().cols());
  _rkOverlay->data()->setRange(QCPRange(0, item->data().rows()/item->Fs()),
                               QCPRange(item->scales()(0), item->scales()(item->scales().size()-1)));
  QCPColorGradient cmap;
  for (int i=0; i<cmap.levelCount(); i++) {
    double a = double(i)/(cmap.levelCount()-1);
    cmap.setColorStopAt(a, QColor(0,0,255,a*255));
  }
  _rkOverlay->setGradient(cmap);
  _rkOverlay->setInterpolate(false);
  _rkOverlay->setVisible(false);

  QSharedPointer<QCPAxisTickerLog> ticker;
  switch (_item->scaling()) {
    case TransformedItem::LINEAR:
      break;
    case TransformedItem::DYADIC:
      ticker = QSharedPointer<QCPAxisTickerLog>(new QCPAxisTickerLog());
      ticker->setLogBase(2);
      yAxis->setTicker(ticker);
      yAxis->setScaleType(QCPAxis::stLogarithmic);
      break;
    case TransformedItem::DECADIC:
      ticker = QSharedPointer<QCPAxisTickerLog>(new QCPAxisTickerLog());
      ticker->setLogBase(10);
      yAxis->setTicker(ticker);
      yAxis->setScaleType(QCPAxis::stLogarithmic);
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
    _curve->setData(QVector<double>(), QVector<double>());
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

  double x = xAxis->pixelToCoord(event->x());
  x = std::max(xAxis->range().lower, x);
  x = std::min(xAxis->range().upper, x);
  double y = yAxis->pixelToCoord(event->y());
  y = std::max(yAxis->range().lower, y);
  y = std::min(yAxis->range().upper, y);

  if (_cropping) {
    // Handle polygon cropping
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
  } else {
    double dt = 1./_item->Fs();
    double b = xAxis->pixelToCoord(event->x());
    double a = yAxis->pixelToCoord(event->y());
    if ((b<xAxis->range().lower) || (b>xAxis->range().upper) || (a<yAxis->range().lower) || (a>yAxis->range().upper)) {
      _rkOverlay->setVisible(false);
    }
    double rval = std::abs(_item->wavelet().evalRepKern(0,1));
    // Plot modulus of rep. kern. at x,y
    for (int i=0; i<_item->data().rows(); i++) {
      for (int j=0; j<_item->data().cols(); j++) {
        double value = std::abs(_item->wavelet().evalRepKern((b-i*dt)/a, _item->scales()(j)/a));
        _rkOverlay->data()->setCell(i, j, value/rval);
      }
    }
    _rkOverlay->rescaleDataRange();
    _rkOverlay->setVisible(true);
  }
  replot();
}

