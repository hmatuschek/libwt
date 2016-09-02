#include "transformdialog.hh"
#include <QDoubleValidator>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QDialogButtonBox>


TransformDialog::TransformDialog(size_t Ns, double Fs, QWidget *parent) :
  QDialog(parent)
{
  _wavelet = new QComboBox();
  _wavelet->addItem(tr("Morlet"), int(MORLET_WAVELET));
  _wavelet->addItem(tr("Cauchy"), int(CAUCHY_WAVELET));

  _scaling = new QComboBox();
  _scaling->addItem(tr("Linear"), int(LINEAR));
  _scaling->addItem(tr("Dyadic"), int(DYADIC));
  _scaling->addItem(tr("Decadic"), int(DECADIC));

  _minScale = new QLineEdit(QString::number(2./Fs));
  QDoubleValidator *val = new QDoubleValidator();
  val->setBottom(2./Fs); val->setTop(Ns/Fs);
  _minScale->setValidator(val);

  _maxScale = new QLineEdit(QString::number(Ns/Fs));
  val = new QDoubleValidator();
  val->setBottom(2./Fs); val->setTop(Ns/Fs);
  _maxScale->setValidator(val);

  _numScales = new QSpinBox();
  _numScales->setValue(32);
  _numScales->setMinimum(1);

  QFormLayout *form = new QFormLayout();
  form->addRow(tr("Wavelet"), _wavelet);
  form->addRow(tr("Scaling"), _scaling);
  form->addRow(tr("Min. scale"), _minScale);
  form->addRow(tr("Max. scale"), _maxScale);
  form->addRow(tr("Num. scales"), _numScales);

  QDialogButtonBox *bb = new QDialogButtonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addLayout(form);
  layout->addWidget(bb);

  setLayout(layout);

  connect(bb, SIGNAL(accepted()), this, SLOT(accept()));
  connect(bb, SIGNAL(rejected()), this, SLOT(reject()));
}

TransformDialog::Wavelet
TransformDialog::wavelet() const {
  return Wavelet(_wavelet->currentData().toInt());
}

TransformDialog::Scaling
TransformDialog::scaling() const {
  return Scaling(_scaling->currentData().toInt());
}

double
TransformDialog::minScale() const {
  return _minScale->text().toDouble();
}

double
TransformDialog::maxScale() const {
  return _maxScale->text().toDouble();
}

size_t
TransformDialog::numScales() const {
  return _numScales->text().toUInt();
}

