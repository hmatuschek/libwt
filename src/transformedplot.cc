#include "transformedplot.hh"
#include "transformeditem.hh"
#include "fmtutil.hh"
#include <iostream>


inline double angle(const std::complex<double> &val) {
  return std::atan2(val.imag(), val.real());
}


/* ********************************************************************************************* *
 * Implementation of TransformedPlot::Settings
 * ********************************************************************************************* */
TransformedPlot::Settings::Settings()
  : _showTitle(true), _showModulus(true), _showRepKern(true), _showVoice(false), _showZoom(false),
    _showWavelet(false)
{
  // pass...
}

TransformedPlot::Settings::Settings(const Settings &other)
  : _showTitle(other._showTitle), _showModulus(other._showModulus), _showRepKern(other._showRepKern),
    _showVoice(other._showVoice), _showZoom(other._showZoom), _showWavelet(other._showWavelet)
{
  // pass...
}

TransformedPlot::Settings &
TransformedPlot::Settings::operator =(const Settings &other) {
  _showTitle = other._showTitle;
  _showModulus = other._showModulus;
  _showRepKern = other._showRepKern;
  _showVoice = other._showVoice;
  _showZoom = other._showZoom;
  _showWavelet = other._showWavelet;
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

bool
TransformedPlot::Settings::showModulus() const {
  return _showModulus;
}

void
TransformedPlot::Settings::setShowModulus(bool show) {
  _showModulus = show;
}

bool
TransformedPlot::Settings::showRepKern() const {
  return _showRepKern;
}

void
TransformedPlot::Settings::setShowRepKern(bool show) {
  _showRepKern = show;
}

bool
TransformedPlot::Settings::showVoice() const {
  return _showVoice;
}

void
TransformedPlot::Settings::setShowVoice(bool show) {
  _showVoice = show;
}

bool
TransformedPlot::Settings::showZoom() const {
  return _showZoom;
}

void
TransformedPlot::Settings::setShowZoom(bool show) {
  _showZoom = show;
}

bool
TransformedPlot::Settings::showWavelet() const {
  return _showWavelet;
}

void
TransformedPlot::Settings::setShowWavelet(bool show) {
  _showWavelet = show;
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
  plotLayout()->insertColumn(0);
  QString title = tr("Wavelet transformed '%0' [Fs=%1]")
      .arg(_item->label(), fmt_freq(_item->Fs()));
  _title = new QCPTextElement(this, title);
  QFont font = _title->font(); font.setPointSize(18);
  _title->setFont(font);
  plotLayout()->addElement(0, 1, _title);
  _title->setVisible(_settings.showTitle());
  xAxis->setLabel("Time [s]");
  yAxis->setLabel("Scale [s]");

  _colorMap = new QCPColorMap(xAxis, yAxis);
  _colorMap->data()->setSize(item->data().rows(), item->data().cols());
  _colorMap->data()->setRange(QCPRange(0, item->data().rows()/item->Fs()),
                              QCPRange(item->scales()(0), item->scales()(item->scales().size()-1)));

  QCPColorScale *colorScale = new QCPColorScale(this);
  plotLayout()->addElement(1, 2, colorScale);
  colorScale->setType(QCPAxis::atRight);
  _colorMap->setColorScale(colorScale);

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

  _leftPaneAxis = new QCPAxisRect(this);
  plotLayout()->addElement(1,0, _leftPaneAxis);
  _bottomPaneAxis = new QCPAxisRect(this);
  plotLayout()->addElement(2,1, _bottomPaneAxis);

  plotLayout()->setColumnStretchFactor(1,7);
  plotLayout()->setRowStretchFactor(1,5);
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

  for (int i=0; i<_item->data().rows(); i++) {
    for (int j=0; j<_item->data().cols(); j++) {
      if (_settings.showModulus())
        _colorMap->data()->setCell(i, j, std::abs(_item->data()(i,j)));
      else
        _colorMap->data()->setCell(i, j, angle(_item->data()(i,j)));
    }
  }

  if (_settings.showModulus()) {
    _colorMap->colorScale()->axis()->setLabel("Modulus [a.u.]");
    _colorMap->colorScale()->axis()->setTicker(QSharedPointer<QCPAxisTicker>(new QCPAxisTicker()));
    _colorMap->setGradient(QCPColorGradient::gpHot);
  } else {
    _colorMap->colorScale()->axis()->setLabel("Phase [rad]");
    _colorMap->colorScale()->axis()->setTicker(QSharedPointer<QCPAxisTicker>(new QCPAxisTickerPi()));
    _colorMap->setGradient(QCPColorGradient::gpHues);
  }
  _colorMap->rescaleDataRange(true);
  rescaleAxes();
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
    } else {
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
  }

  // Replot anyway...
  replot();
}

