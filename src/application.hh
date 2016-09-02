#ifndef APPLICATION_HH
#define APPLICATION_HH

#include <QApplication>
#include "item.hh"

class Application: public QApplication
{
  Q_OBJECT

public:
  Application(int &argc, char **argv);
  virtual ~Application();

  inline ItemModel *items() { return _items; }

public slots:
  void importTimeseries();

protected:
  ItemModel *_items;
};

#endif // APPLICATION_HH
