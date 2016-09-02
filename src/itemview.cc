#include "itemview.hh"


/* ********************************************************************************************* *
 * Implementation of TimeseriesItemView
 * ********************************************************************************************* */
TimeseriesItemView::TimeseriesItemView(TimeseriesItem *item, QWidget *parent)
  : QCustomPlot(parent), _item(item)
{
  addGraph();
  double t=0, dt = 1/_item->Fs();
  for (int i=0; i<_item->data().size(); i++, t+=dt) {
    graph(0)->addData(t, _item->data()(i));
  }
  xAxis->setLabel(tr("Time [s]"));
  yAxis->setLabel(tr("Value [a.u.]"));
  graph(0)->rescaleAxes();
}



/* ********************************************************************************************* *
 * Implementation of ItemListView
 * ********************************************************************************************* */
ItemListView::ItemListView(Application &app, QWidget *parent)
  : QListView(parent), _application(app)
{
  setModel(_application.items());
  _importButton = new QPushButton(tr("Import time series..."), this);
  layoutImportButton();
  _importButton->setVisible(false);

  connect(_application.items(), SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(_onItemAdded()));
  connect(_application.items(), SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(_onItemRemoved()));
  connect(_importButton, SIGNAL(clicked(bool)), &_application, SLOT(importTimeseries()));

  _onItemRemoved();
}

void
ItemListView::resizeEvent(QResizeEvent *e) {
  QListView::resizeEvent(e);
  layoutImportButton();
}

void
ItemListView::layoutImportButton() {
  int w = width(), h = height();
  int bw = _importButton->width(), bh = _importButton->height();
  int x = (w-bw)/2, y = (h-bh)/2;
  _importButton->move(x,y);
}

void
ItemListView::_onItemAdded() {
  _importButton->setVisible(false);
}

void
ItemListView::_onItemRemoved() {
  if (0 == model()->rowCount())
    _importButton->setVisible(true);
}
