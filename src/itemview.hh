#ifndef ITEMVIEW_HH
#define ITEMVIEW_HH

#include <QListView>
#include <QPushButton>
#include <QSortFilterProxyModel>
#include "application.hh"
#include "qcustomplot.hh"

typedef enum {
  ItemFilter_All,
  ItemFilter_Timeseries,
  ItemFilter_Transformed,
  ItemFilter_Tasks
} ItemFilter;


class ItemListView: public QListView
{
  Q_OBJECT

public:
  ItemListView(Application &app, QWidget *parent=0);

public slots:
  void filter(ItemFilter filter);

signals:
  void itemSelected(size_t idx);

protected slots:
  void _onItemAdded();
  void _onItemRemoved();
  void _onItemSelected(QItemSelection selected, QItemSelection deselected);

protected:
  void resizeEvent(QResizeEvent *e);
  void layoutImportButton();

protected:
  Application &_application;
  QPushButton *_importButton;
  QSortFilterProxyModel *_proxy;
};


class ItemFilterSelectionWidget: public QComboBox
{
  Q_OBJECT

public:
  ItemFilterSelectionWidget(QWidget *parent=0);

signals:
  void filterSelected(ItemFilter filter);

protected slots:
  void onCurrentIndexChanged(int idx);
};


#endif // ITEMVIEW_HH
