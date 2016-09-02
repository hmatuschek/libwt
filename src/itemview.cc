#include "itemview.hh"

ItemView::ItemView(Application &app, QWidget *parent)
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
ItemView::resizeEvent(QResizeEvent *e) {
  QListView::resizeEvent(e);
  layoutImportButton();
}

void
ItemView::layoutImportButton() {
  int w = width(), h = height();
  int bw = _importButton->width(), bh = _importButton->height();
  int x = (w-bw)/2, y = (h-bh)/2;
  _importButton->move(x,y);
}

void
ItemView::_onItemAdded() {
  _importButton->setVisible(false);
}

void
ItemView::_onItemRemoved() {
  if (0 == model()->rowCount())
    _importButton->setVisible(true);
}
