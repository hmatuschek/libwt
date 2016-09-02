#ifndef ITEMVIEW_HH
#define ITEMVIEW_HH

#include <QListView>
#include <QPushButton>
#include "application.hh"
#include "qcustomplot.hh"

class TimeseriesItemView: public QCustomPlot
{
  Q_OBJECT

public:
  TimeseriesItemView(TimeseriesItem *item, QWidget *parent=0);

protected:
  TimeseriesItem *_item;
};


class ItemListView: public QListView
{
  Q_OBJECT

public:
  ItemListView(Application &app, QWidget *parent=0);

protected slots:
  void _onItemAdded();
  void _onItemRemoved();

protected:
  void resizeEvent(QResizeEvent *e);
  void layoutImportButton();

protected:
  Application &_application;
  QPushButton *_importButton;
};



#endif // ITEMVIEW_HH
