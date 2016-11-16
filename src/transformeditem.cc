#include "transformeditem.hh"
#include "fmtutil.hh"
#include "application.hh"
#include "transformedplot.hh"
#include <QInputDialog>
#include <fstream>
#include "utils/csv.hh"


/* ******************************************************************************************** *
 * Implementation of TransformedItem
 * ******************************************************************************************** */
TransformedItem::TransformedItem(const wt::Wavelet &wavelet, double Fs, const Eigen::Ref<const Eigen::VectorXd> &scales, Scaling scaling,
    const Eigen::Ref<const Eigen::MatrixXcd> &data, const QString &label, QObject *parent)
  : Item(parent), _wavelet(wavelet), _Fs(Fs), _scales(scales), _scaling(scaling), _data(data)
{
  _label = label;
  _icon = QIcon("://icons/wavelet16.png");
}

TransformedItem::~TransformedItem() {
  // pass...
}

QWidget *
TransformedItem::view() {
  QWidget *view = new TransformedItemView(this);
  connect(this, SIGNAL(destroyed()), view, SLOT(deleteLater()));
  return view;
}


/* ********************************************************************************************* *
 * Implementation of TransformedItemView
 * ********************************************************************************************* */
TransformedItemView::TransformedItemView(TransformedItem *item, QWidget *parent)
  : QWidget(parent), _item(item)
{
  _plot = new TransformedPlot(_item, TransformedPlot::Settings(), this);
  connect(_plot, SIGNAL(cropped(Polygon)), this, SLOT(cropped(Polygon)));

  QToolBar *toolbar = new QToolBar();
  QAction *a = toolbar->addAction(QIcon("://icons/synthesis32.png"), "", this, SLOT(doSynthesis()));
  a->setToolTip(tr("Perform wavelet synthesis."));

  a = toolbar->addAction(QIcon("://icons/modulus32.png"), "", this, SLOT(doProjection()));
  a->setToolTip(tr("Perform projection."));
  toolbar->addSeparator();

  _cropAction = toolbar->addAction(QIcon("://icons/crop24.png"), "");
  _cropAction->setToolTip(tr("Crop wavelet transformed."));
  _cropAction->setCheckable(true); _cropAction->setChecked(false);
  connect(_cropAction, SIGNAL(toggled(bool)), _plot, SLOT(crop(bool)));

  QAction *funcAction = new QAction(QIcon("://icons/cog24.png"), "", 0);
  QMenu *menu = new QMenu();
  menu->addAction(QIcon("://icons/cog16.png"), tr("Truncate by amplitude"), this, SLOT(truncate()));
  funcAction->setMenu(menu);
  toolbar->addAction(funcAction);

  a = toolbar->addAction(QIcon("://icons/wrench24.png"), "", this, SLOT(config()));
  a->setToolTip(tr("Config item & view ..."));
  toolbar->addSeparator();

  a = new QAction(QIcon("://icons/data-transfer-download24.png"), "", 0);
  menu = new QMenu();
  menu->addAction(QIcon("://icons/data-transfer-download24.png"),
                  tr("Save plot ..."), this, SLOT(savePlot()));
  menu->addAction(QIcon("://icons/data-transfer-download24.png"),
                  "Export transformed ...", this, SLOT(saveData()));
  a->setMenu(menu);
  toolbar->addAction(a);

  QWidget *spacer = new QWidget();
  spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
  toolbar->addWidget(spacer);

  a = toolbar->addAction(QIcon("://icons/trash24.png"), "", this, SLOT(delItem()));
  a->setToolTip(tr("Delete wavelet transformed."));

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(toolbar);
  layout->addWidget(_plot);
  layout->setContentsMargins(0,0,0,0);
  layout->setSpacing(0);
  setLayout(layout);
}

void
TransformedItemView::doSynthesis() {
  Application *app = qobject_cast<Application *>(QApplication::instance());
  app->startSynthesis(_item);
}

void
TransformedItemView::doProjection() {
  Application *app = qobject_cast<Application *>(QApplication::instance());
  app->startProjection(_item);
}

void
TransformedItemView::truncate() {
  bool ok;
  double thres = QInputDialog::getDouble(
        0, tr("Truncate by amplitude."), tr("Specify the truncation threshold."),
        0, 0, 2e100, 5, &ok);
  if (! ok) return;

  Eigen::MatrixXcd tmp(_item->data().rows(), _item->data().cols());
  for (int i=0; i<_item->data().rows(); i++) {
    for (int j=0; j<_item->data().cols(); j++) {
      if (std::abs(_item->data()(i,j)) >= thres)
        tmp(i,j) = _item->data()(i,j);
      else
        tmp(i,j) = 0;
    }
  }

  QString label = QInputDialog::getText(
        0, tr("Time series label"), tr("Select a new label for the detrended time series."),
        QLineEdit::Normal, _item->label(), &ok);
  if (! ok) return;

  Application *app = qobject_cast<Application *>(QApplication::instance());
  app->items()->addItem(new TransformedItem(_item->wavelet(), _item->Fs(), _item->scales(),
                                            _item->scaling(), tmp, label));
}

void
TransformedItemView::config() {
  Application *app = qobject_cast<Application *>(QApplication::instance());
  TransformedItemViewConfigDialog dialog(app, _item->label(), _plot->settings());
  if (QDialog::Rejected == dialog.exec())
    return;
  _item->setLabel(dialog.label());
  _plot->applySettings(dialog.plotSettings());
}

void
TransformedItemView::savePlot() {
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
TransformedItemView::saveData() {
  QMessageBox::information(
        0, tr("Cannot save transformed."),
        tr("Cannot save transformed: Not implemented yet."));
}

void
TransformedItemView::delItem() {
  Application *app = qobject_cast<Application *>(QApplication::instance());
  app->items()->remItem(_item);
}

void
TransformedItemView::cropped(const Polygon &poly) {
  _cropAction->setChecked(false);

  if ((poly.size() < 3) || ! poly.isClosed())
    return;

  Eigen::MatrixXcd tmp(_item->data().rows(), _item->data().cols());
  for (int i=0; i<tmp.rows(); i++) {
    for (int j=0; j<tmp.cols(); j++) {
      double x = double(i)/_item->Fs();
      double y = _item->scales()(j);
      if (poly.inside(x,y))
        tmp(i,j) = _item->data()(i,j);
      else
        tmp(i,j) = 0;
    }
  }

  bool ok;
  QString label = QInputDialog::getText(
        0, tr("Enter label"), tr("Enter the label of the cropped transformed:"),
        QLineEdit::Normal, _item->label(), &ok);
  if (! ok)
    return;

  Application *app = qobject_cast<Application *>(QApplication::instance());
  app->items()->addItem(
        new TransformedItem(
          _item->wavelet(), _item->Fs(), _item->scales(), _item->scaling(),
          tmp, label));
}


/* ********************************************************************************************* *
 * Implementation of TransformedItemViewConfigDialog
 * ********************************************************************************************* */
TransformedItemViewConfigDialog::TransformedItemViewConfigDialog(
    Application *app, const QString &label, const TransformedPlot::Settings &plotSettings, QWidget *parent)
  : QDialog(parent), _application(app), _oldLabel(label)
{
  _showTitle = new QCheckBox();
  _showTitle->setChecked(plotSettings.showTitle());
  _showModulus = new QCheckBox();
  _showModulus->setChecked(plotSettings.showModulus());
  _label = new QLineEdit(label);

  QFormLayout *form = new QFormLayout();
  form->addRow(tr("Show title"), _showTitle);
  form->addRow(tr("Show modulus/phase"), _showModulus);
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
TransformedItemViewConfigDialog::label() const {
  return _label->text();
}

TransformedPlot::Settings
TransformedItemViewConfigDialog::plotSettings() const {
  TransformedPlot::Settings settings;
  settings.setShowTitle(_showTitle->isChecked());
  settings.setShowModulus(_showModulus->isChecked());
  return settings;
}

void
TransformedItemViewConfigDialog::accept() {
  QString label = _label->text();
  if ((_oldLabel != label) && _application->items()->contains(label)) {
    QMessageBox::information(
          0, tr("Choose a different label."),
          tr("There is already an item labeled '%0', chosse a different one.").arg(label));
    return;
  }
  QDialog::accept();
}
