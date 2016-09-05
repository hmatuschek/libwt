#include "itemview.hh"
#include "fmtutil.hh"
#include <QVBoxLayout>
#include <QToolBar>
#include "item.hh"

/* ********************************************************************************************* *
 * Implementation of ItemListView
 * ********************************************************************************************* */
ItemListView::ItemListView(Application &app, QWidget *parent)
  : QListView(parent), _application(app)
{
  _proxy = new QSortFilterProxyModel();
  _proxy->setSourceModel(_application.items());
  _proxy->setFilterRole(Qt::UserRole);
  _proxy->setFilterWildcard("*");
  setModel(_proxy);

  _importButton = new QPushButton(tr("Import time series..."), this);
  layoutImportButton();
  _importButton->setVisible(false);

  connect(_application.items(), SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(_onItemAdded()));
  connect(_application.items(), SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(_onItemRemoved()));
  connect(_importButton, SIGNAL(clicked(bool)), &_application, SLOT(importTimeseries()));
  connect(this->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
          this, SLOT(_onItemSelected(QItemSelection,QItemSelection)));

  _onItemRemoved();
}

void
ItemListView::filter(ItemFilter filter) {
  switch (filter) {
    case ItemFilter_All:
      _proxy->setFilterWildcard("*");
      break;
    case ItemFilter_Timeseries:
      _proxy->setFilterWildcard("0");
      break;
    case ItemFilter_Transformed:
      _proxy->setFilterWildcard("1");
      break;
    case ItemFilter_Tasks:
      _proxy->setFilterWildcard("2");
      break;
  }
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
  if (0 == _proxy->sourceModel()->rowCount())
    _importButton->setVisible(true);
}

void
ItemListView::_onItemSelected(QItemSelection selected, QItemSelection deselected) {
  if (0 == selected.count()) return;
  QModelIndex idx = _proxy->mapToSource(selected.indexes().first());
  emit itemSelected(idx.row());
}


/* ********************************************************************************************* *
 * Implementation of ItemFilterSelectionWidget
 * ********************************************************************************************* */
ItemFilterSelectionWidget::ItemFilterSelectionWidget(QWidget *parent)
  : QComboBox(parent)
{
  addItem(tr("All"), ItemFilter_All);
  addItem(tr("Time series"), ItemFilter_Timeseries);
  addItem(tr("Transformed"), ItemFilter_Transformed);
  addItem(tr("Tasks"), ItemFilter_Tasks);
  connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentIndexChanged(int)));
}

void
ItemFilterSelectionWidget::onCurrentIndexChanged(int idx) {
  emit filterSelected(ItemFilter(this->currentData().toUInt()));
}
