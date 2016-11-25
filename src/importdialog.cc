#include "importdialog.hh"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QDoubleValidator>
#include <QFileInfo>
#include <QDialogButtonBox>
#include <QMessageBox>
#include "application.hh"
#include "item.hh"


ImportDialog::ImportDialog(const QString &filename, size_t cols, Application &app, QWidget *parent)
  : QDialog(parent), _application(app)
{
  _typeSel = new QComboBox();
  _typeSel->addItem(tr("real"));
  if (cols>1) _typeSel->addItem(tr("complex"));
  _typeSel->setCurrentIndex(0);

  _realCol = new QSpinBox();
  _realCol->setRange(0, cols-1);

  _imagCol = new QSpinBox();
  _imagCol->setRange(0, cols-1);
  _imagCol->setEnabled(false);

  _Fs = new QLineEdit("1.0");
  QDoubleValidator *dval = new QDoubleValidator(0, 1e12, 3);
  _Fs->setValidator(dval);

  _t0 = new QLineEdit("0.0");
  dval = new QDoubleValidator();
  _t0->setValidator(dval);

  QFileInfo finfo(filename);
  QString name = finfo.baseName();
  _label = new QLineEdit(name);

  QVBoxLayout *layout = new QVBoxLayout();

  QFormLayout *form = new QFormLayout();
  form->addRow(tr("Data type"), _typeSel);
  form->addRow(tr("Column (real)"), _realCol);
  form->addRow(tr("Column (imag)"), _imagCol);
  form->addRow(tr("Sample rate [1/s]"), _Fs);
  form->addRow(tr("Initial time [s]"), _t0);
  form->addRow(tr("Label"), _label);

  QDialogButtonBox *bb = new QDialogButtonBox(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
  layout->addLayout(form);
  layout->addWidget(bb);
  setLayout(layout);

  connect(_typeSel, SIGNAL(currentIndexChanged(int)), this, SLOT(_onTypeSelected(int)));
  connect(bb, SIGNAL(accepted()), this, SLOT(verify()));
  connect(bb, SIGNAL(rejected()), this, SLOT(reject()));
}

bool
ImportDialog::real() const {
  return 0 == _typeSel->currentIndex();
}

size_t
ImportDialog::realColumn() const {
  return _realCol->value();
}

size_t
ImportDialog::imagColumn() const {
  return _imagCol->value();
}

double
ImportDialog::Fs() const {
  return _Fs->text().toDouble();
}

double
ImportDialog::t0() const {
  return _t0->text().toDouble();
}

QString
ImportDialog::label() const {
  return _label->text();
}

void
ImportDialog::_onTypeSelected(int idx) {
  _imagCol->setEnabled(1 == idx);
}

void
ImportDialog::verify() {
  if (_application.items()->contains(_label->text().simplified())) {
    QMessageBox::critical(0, tr("Label in use."), tr("The label '%0' is alrady is use.").arg(_label->text().simplified()));
  } else {
    accept();
  }
}
