#include "timeseriesitem.hh"
#include <QToolBar>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include "application.hh"
#include "timeseriesplot.hh"
#include "utils/csv.hh"
#include "detrend.hh"
#include <fstream>


/* ******************************************************************************************** *
 * Implementation of TimeseriesItem
 * ******************************************************************************************** */
TimeseriesItem::TimeseriesItem(const Eigen::Ref<const Eigen::VectorXd> &data, double Fs,
                               const QString &label, QObject *parent)
  : Item(parent), _data(data), _Fs(Fs)
{
  _label = label;
  _icon  = QIcon("://icons/timeseries16.png");
}

TimeseriesItem::~TimeseriesItem() {
  // pass...
}

QWidget *
TimeseriesItem::view() {
  QWidget *view = new TimeseriesItemView(this);
  connect(this, SIGNAL(destroyed()), view, SLOT(deleteLater()));
  return view;
}


/* ********************************************************************************************* *
 * Implementation of TimeseriesItemView
 * ********************************************************************************************* */
TimeseriesItemView::TimeseriesItemView(TimeseriesItem *item, QWidget *parent)
  : QWidget(parent), _item(item)
{
  _plot = new TimeseriesPlot(_item, TimeseriesPlot::Settings(), this);
  connect(_plot, SIGNAL(cropped(double,double)), this, SLOT(cropped(double,double)));

  QToolBar *toolbar = new QToolBar();
  QAction *a = toolbar->addAction(QIcon("://icons/wavelet32.png"), "", this, SLOT(doTransform()));
  a->setToolTip(tr("Perform wavelet transform of signal."));
  toolbar->addSeparator();

  _cropAction = toolbar->addAction(QIcon("://icons/crop24.png"), "");
  _cropAction->setToolTip(tr("Crop time series."));
  _cropAction->setCheckable(true); a->setChecked(false);
  connect(_cropAction, SIGNAL(toggled(bool)), _plot, SLOT(crop(bool)));

  QAction *funcAction = new QAction(QIcon("://icons/cog24.png"), "", 0);
  QMenu *menu = new QMenu();
  menu->addAction(QIcon("://icons/cog16.png"), tr("Detrend timeseries"), this, SLOT(detrend()));
  funcAction->setMenu(menu);
  toolbar->addAction(funcAction);

  a = toolbar->addAction(QIcon("://icons/wrench24.png"), "", this, SLOT(config()));
  a->setToolTip(tr("Config item and plot ..."));
  toolbar->addSeparator();

  a = new QAction(QIcon("://icons/data-transfer-download24.png"), "", 0);
  menu = new QMenu();
  menu->addAction(QIcon("://icons/data-transfer-download24.png"),
                  tr("Save plot ..."), this, SLOT(savePlot()));
  menu->addAction(QIcon("://icons/data-transfer-download24.png"),
                  "Export timeseries ...", this, SLOT(saveData()));
  a->setMenu(menu);
  toolbar->addAction(a);

  QWidget *spacer = new QWidget();
  spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
  toolbar->addWidget(spacer);

  a = toolbar->addAction(QIcon("://icons/trash24.png"), "", this, SLOT(delItem()));
  a->setToolTip(tr("Delete time series."));

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(toolbar);
  layout->addWidget(_plot);
  layout->setContentsMargins(0,0,0,0);
  layout->setSpacing(0);
  setLayout(layout);
}

void
TimeseriesItemView::doTransform() {
  Application *app = qobject_cast<Application *>(QApplication::instance());
  app->startTransform(_item);
}

void
TimeseriesItemView::cropped(double x1, double x2) {
  bool ok;
  QString label = QInputDialog::getText(
        0, tr("Time series label"), tr("Select a new label for the cropped time series."),
        QLineEdit::Normal, _item->label(), &ok);

  _cropAction->setChecked(false);

  if (! ok)
    return;

  Application *app = qobject_cast<Application *>(QApplication::instance());
  int i1 = x1*_item->Fs(), i2 = x2*_item->Fs();
  app->addTimeseries(label, _item->data().segment(i1, i2-i1), _item->Fs());
}

void
TimeseriesItemView::detrend() {
  bool ok;
  QString label = QInputDialog::getText(
        0, tr("Time series label"), tr("Select a new label for the detrended time series."),
        QLineEdit::Normal, _item->label(), &ok);
  if (! ok)
    return;
  TimeseriesItem *item = new TimeseriesItem(_item->data(), _item->Fs(), label);
  wt::detrend(item->data());
  Application *app = qobject_cast<Application *>(QApplication::instance());
  app->items()->addItem(item);
}

void
TimeseriesItemView::config() {
  Application *app = qobject_cast<Application *>(QApplication::instance());
  TimeseriesItemViewConfigDialog dialog(app, _item->label(), _plot->settings());
  if (QDialog::Rejected == dialog.exec())
    return;
  _item->setLabel(dialog.label());
  _plot->applySettings(dialog.plotSettings());
}

void
TimeseriesItemView::savePlot() {
  QString filename = QFileDialog::getSaveFileName(
        0, tr("Save plot as"), "", tr("Portable Document Format (*.pdf);;"
                                      "Portable Network Graphics (*.png)"));

  if (filename.isEmpty())
    return;

  QFileInfo finfo(filename);

  if (filename.endsWith("png")) {
    _plot->savePng(filename);
  } else if (filename.endsWith("pdf")) {
    _plot->savePdf(filename);
  } else {
    QMessageBox::critical(
          0, tr("Cannot save plot."),
          tr("Cannot save plot as '%1', format not supported.").arg(finfo.fileName()));
  }
}

void
TimeseriesItemView::saveData() {
  QString filename = QFileDialog::getSaveFileName(
        0, tr("Export timeseries to"), "", tr("Comma Separated Values (*.csv)"));

  if (filename.isEmpty())
    return;

  QFileInfo finfo(filename);
  std::ofstream file;
  file.open(filename.toStdString());
  if (! file.is_open()) {
    QMessageBox::critical(
          0, tr("Cannot save timeseries ."),
          tr("Cannot save timeseries as '%1', cannot open file.").arg(finfo.fileName()));
    return;
  }
  wt::CSV::write(_item->data(), file);
  file.close();
}

void
TimeseriesItemView::delItem() {
  Application *app = qobject_cast<Application *>(QApplication::instance());
  app->items()->remItem(_item);
}


/* ********************************************************************************************* *
 * Implementation of TimeseriesItemViewConfigDialog
 * ********************************************************************************************* */
TimeseriesItemViewConfigDialog::TimeseriesItemViewConfigDialog(Application *app,
    const QString &label, const TimeseriesPlot::Settings &settings, QWidget *parent)
  : QDialog(parent), _application(app)
{
  _oldLabel = label;
  _label = new QLineEdit(label);

  QFormLayout *form = new QFormLayout();
  form->addRow(tr("Label"), _label);

  QDialogButtonBox *bb = new QDialogButtonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);
  QVBoxLayout *layout = new QVBoxLayout();
  layout->addLayout(form);
  layout->addWidget(bb);
  setLayout(layout);

  connect(bb, SIGNAL(accepted()), this, SLOT(accept()));
  connect(bb, SIGNAL(rejected()), this, SLOT(reject()));
}

QString
TimeseriesItemViewConfigDialog::label() const {
  return _label->text();
}

TimeseriesPlot::Settings
TimeseriesItemViewConfigDialog::plotSettings() const {
  TimeseriesPlot::Settings settings;
  return settings;
}

void
TimeseriesItemViewConfigDialog::accept() {
  QString label = _label->text();
  if ((_oldLabel != label) && _application->items()->contains(label)) {
    QMessageBox::information(
          0, tr("Choose a different label."),
          tr("There is already an item labeled '%0', chosse a different one.").arg(label));
    return;
  }
  QDialog::accept();
}
