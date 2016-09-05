#include "timeseriesplot.hh"
#include "timeseriesitem.hh"
#include "fmtutil.hh"


/* ********************************************************************************************* *
 * Implementation of TimeseriesPlot::Settings
 * ********************************************************************************************* */
TimeseriesPlot::Settings::Settings()
  : _showTitle(true)
{
  // pass...
}

TimeseriesPlot::Settings::Settings(const Settings &other)
  : _showTitle(other._showTitle)
{
  // pass...
}

TimeseriesPlot::Settings &
TimeseriesPlot::Settings::operator =(const Settings &other) {
  _showTitle = other._showTitle;
  return *this;
}

bool
TimeseriesPlot::Settings::showTitle() const {
  return _showTitle;
}

void
TimeseriesPlot::Settings::setShowTitle(bool show) {
  _showTitle = show;
}


/* ********************************************************************************************* *
 * Implementation of TimeseriesPlot
 * ********************************************************************************************* */
TimeseriesPlot::TimeseriesPlot(TimeseriesItem *item, const Settings &settings, QWidget *parent)
  :QCustomPlot(parent), _item(item), _title(0), _settings(settings),
    _cropping(false), _dragging(false), _selection(0)
{
  addGraph();

  _selection = new QCPItemRect(this);
  addItem(_selection);
  _selection->setVisible(false);
  _selection->setPen(QPen(Qt::transparent));
  _selection->setBrush(QColor(0, 0, 255, 32));

  double t=0, dt = 1/_item->Fs();
  for (int i=0; i<_item->data().size(); i++, t+=dt) {
    graph(0)->addData(t, _item->data()(i));
  }
  plotLayout()->insertRow(0);
  QString title = tr("Timeseries '%0' [Fs=%1]").arg(_item->label(), fmt_freq(_item->Fs()));
  _title = new QCPPlotTitle(this, title);
  plotLayout()->addElement(0, 0, _title);
  _title->setVisible(settings.showTitle());
  xAxis->setLabel(tr("Time [s]"));
  yAxis->setLabel(tr("Value [a.u.]"));
  graph(0)->rescaleAxes();

  _selection->topLeft->setCoords(xAxis->range().lower, yAxis->range().upper);
  _selection->bottomRight->setCoords(xAxis->range().lower, yAxis->range().lower);
}

const TimeseriesPlot::Settings &
TimeseriesPlot::settings() const {
  return _settings;
}

void
TimeseriesPlot::applySettings(const Settings &settings) {
  _settings = settings;
  QString title = tr("Timeseries '%0' [Fs=%1]").arg(_item->label(), fmt_freq(_item->Fs()));
  _title->setText(title);
  _title->setVisible(settings.showTitle());
  replot();
}

void
TimeseriesPlot::crop(bool crop) {
  _cropping = crop;
  if (_cropping)
    return;
  _selection->setVisible(false);
  replot();
}

void
TimeseriesPlot::mousePressEvent(QMouseEvent *event) {
  QCustomPlot::mousePressEvent(event);

  if (! _cropping)
    return;

  double x1 = xAxis->pixelToCoord(event->x());
  x1 = std::max(xAxis->range().lower, x1);
  x1 = std::min(xAxis->range().upper, x1);
  _selection->topLeft->setCoords(x1, yAxis->range().upper);
  _selection->bottomRight->setCoords(x1, yAxis->range().lower);
  _selection->setVisible(true);
  _dragging = true;
}

void
TimeseriesPlot::mouseMoveEvent(QMouseEvent *event) {
  QCustomPlot::mouseMoveEvent(event);

  if (! _dragging)
    return;

  double x1 = xAxis->pixelToCoord(event->x());
  x1 = std::max(xAxis->range().lower, x1);
  x1 = std::min(xAxis->range().upper, x1);
  _selection->bottomRight->setCoords(x1, yAxis->range().lower);
  replot();
}

void
TimeseriesPlot::mouseReleaseEvent(QMouseEvent *event) {
  QCustomPlot::mouseReleaseEvent(event);

  if (! _dragging)
    return;

  double x1 = xAxis->pixelToCoord(event->x());
  x1 = std::max(xAxis->range().lower, x1);
  x1 = std::min(xAxis->range().upper, x1);
  _selection->bottomRight->setCoords(x1, yAxis->range().lower);
  replot();

  double x2 = _selection->topLeft->coords().x();
  x2 = std::max(xAxis->range().lower, x2);
  x2 = std::min(xAxis->range().upper, x2);

  _dragging = false;

  if (x1 == x2)
    return;

  emit cropped(std::min(x1,x2), std::max(x1, x2));
}
