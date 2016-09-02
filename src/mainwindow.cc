#include "mainwindow.hh"
#include "itemview.hh"
#include <QSplitter>
#include <QLabel>

MainWindow::MainWindow(Application &app, QWidget *parent)
  : QMainWindow(parent), _application(app)
{
  setWindowTitle(tr("Wavelet Tool"));
  setMinimumSize(640,480);
  resize(800, 480);

  ItemListView *itemview = new ItemListView(_application);

  _viewstack = new QStackedWidget();
  QLabel *label = new QLabel(tr("Select an item from the list on the left..."));
  label->setAlignment(Qt::AlignCenter);
  QFont font = label->font();
  font.setBold(true);
  label->setFont(font);
  _viewstack->addWidget(label);

  QSplitter *splitter = new QSplitter();
  splitter->addWidget(itemview);
  splitter->addWidget(_viewstack);
  splitter->setSizes(QList<int> {200, 600});
  setCentralWidget(splitter);

  connect(itemview->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
          this, SLOT(selectedItemChanged(QItemSelection,QItemSelection)));
}

void
MainWindow::selectedItemChanged(const QItemSelection &current, const QItemSelection &previous) {
  if (_viewstack->currentIndex()) {
    QWidget *view = _viewstack->currentWidget();
    _viewstack->removeWidget(view);
    view->deleteLater();
  }

  if (0 == current.count())
    return;

  Item *item = _application.items()->item(current.indexes().first().row());
  QWidget *view = item->view();
  if (view)
    _viewstack->insertWidget(0, view);
  _viewstack->setCurrentIndex(0);
}
