#include "mainwindow.hh"
#include "item.hh"
#include "itemview.hh"
#include <QSplitter>
#include <QLabel>
#include <QVBoxLayout>
#include <QToolBar>
#include <QComboBox>
#include <QMenu>
#include <QShortcut>
#include <QTextBrowser>
#include "fmtutil.hh"

#include "loghandler.hh"


MainWindow::MainWindow(Application &app, QWidget *parent)
  : QMainWindow(parent), _application(app)
{
  setWindowTitle(tr("Wavelet Tool"));
  setMinimumSize(640,480);
  resize(800, 480);

  LogHandler *logHandler = new LogHandler();
  wt::Logger::addHandler(logHandler);

  QWidget *leftPane = new QWidget();
  QVBoxLayout *paneLayout = new QVBoxLayout();
  paneLayout->setContentsMargins(0,0,0,0);
  paneLayout->setSpacing(0);
  leftPane->setLayout(paneLayout);

  QToolBar *toolbar = new QToolBar();
  toolbar->setIconSize(QSize(16,16));
  paneLayout->addWidget(toolbar);

  ItemFilterSelectionWidget *filter = new ItemFilterSelectionWidget();
  filter->setToolTip(tr("Filter items."));
  toolbar->addWidget(filter);

  QAction *a = new QAction(QIcon("://icons/data-transfer-upload16.png"), "", 0);
  a->setMenu(new QMenu());
  a->menu()->addAction(QIcon("://icons/data-transfer-upload16.png"), tr("Import time series ..."),
                  &app, SLOT(importTimeseries()));
  a->menu()->addAction(QIcon("://icons/folder16.png"), tr("Open session ..."),
                       &app, SLOT(loadSession()));
  toolbar->addAction(a);

  a = toolbar->addAction(
        QIcon("://icons/data-transfer-download16.png"), "", &app, SLOT(saveSession()));
  a->setToolTip(tr("Save session ..."));

  ItemListView *itemview = new ItemListView(_application, leftPane);
  paneLayout->addWidget(itemview);
  connect(filter, SIGNAL(filterSelected(ItemFilter)), itemview, SLOT(filter(ItemFilter)));

  _viewstack = new QStackedWidget();
  QTextBrowser *label = new QTextBrowser();
  QFile about("://about.html"); about.open(QIODevice::ReadOnly);
  label->setHtml(about.readAll());
  _viewstack->addWidget(label);

  QSplitter *splitter = new QSplitter();
  splitter->addWidget(leftPane);
  splitter->addWidget(_viewstack);
  splitter->setSizes(QList<int> {220, 580});
  setCentralWidget(splitter);

  _statusBar = new QStatusBar();
  setStatusBar(_statusBar);

  connect(&_application, SIGNAL(procStats(double,double,double)),
          this, SLOT(procStatUpdate(double,double,double)));
  connect(logHandler, SIGNAL(message(QString)), _statusBar, SLOT(showMessage(QString)));
  connect(itemview, SIGNAL(itemSelected(size_t)), this, SLOT(selectedItemChanged(size_t)));

  QShortcut *importAction = new QShortcut(Qt::CTRL+Qt::Key_I, this);
  importAction->setContext(Qt::ApplicationShortcut);
  connect(importAction, SIGNAL(activated()), &app, SLOT(importTimeseries()));
  QShortcut *saveSessionAction = new QShortcut(Qt::CTRL+Qt::Key_S, this);
  saveSessionAction->setContext(Qt::ApplicationShortcut);
  connect(saveSessionAction, SIGNAL(activated()), &app, SLOT(saveSession()));
  QShortcut *openSessionAction = new QShortcut(Qt::CTRL+Qt::Key_O, this);
  openSessionAction->setContext(Qt::ApplicationShortcut);
  connect(openSessionAction, SIGNAL(activated()), &app, SLOT(loadSession()));
  QShortcut *addSessionAction = new QShortcut(Qt::CTRL+Qt::SHIFT+Qt::Key_O, this);
  addSessionAction->setContext(Qt::ApplicationShortcut);
  connect(addSessionAction, SIGNAL(activated()), &app, SLOT(addSession()));
}

void
MainWindow::selectedItemChanged(size_t idx) {
  if (_viewstack->currentIndex()) {
    QWidget *view = _viewstack->currentWidget();
    _viewstack->removeWidget(view);
    view->deleteLater();
  }

  Item *item = _application.items()->item(idx);
  QWidget *view = item->view();
  if (view)
    _viewstack->insertWidget(0, view);
  _viewstack->setCurrentIndex(0);
}

void
MainWindow::procStatUpdate(double mem, double cpu, double time) {
  _statusBar->showMessage(
        tr("CPU: %0 / MEM: %1 / TIME: %2").arg(fmt_cpu(cpu), fmt_mem(mem), fmt_time(time)), 0);
}
