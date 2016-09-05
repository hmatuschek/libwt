#include "transformdialog.hh"
#include <QDoubleValidator>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QDialogButtonBox>


TransformDialog::TransformDialog(size_t Ns, double Fs, QWidget *parent) :
  QDialog(parent)
{
  _scaling = new QComboBox();
  _scaling->addItem(tr("Linear"), int(TransformedItem::LINEAR));
  _scaling->addItem(tr("Dyadic"), int(TransformedItem::DYADIC));
  _scaling->addItem(tr("Decadic"), int(TransformedItem::DECADIC));
  _scaling->setCurrentIndex(2);

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
  _numScales->setMaximum(Ns/2);

  _wavelet = new QComboBox();
  _wavelet->addItem(tr("Morlet"), int(MORLET_WAVELET));
  _wavelet->addItem(tr("Cauchy"), int(CAUCHY_WAVELET));
  _wavelet->setCurrentIndex(1);

  _param1 = new QLineEdit("20");
  val = new QDoubleValidator();
  val->setBottom(0);
  _param1->setValidator(val);

  QFormLayout *form = new QFormLayout();
  form->addRow(tr("Scaling"), _scaling);
  form->addRow(tr("Min. scale"), _minScale);
  form->addRow(tr("Max. scale"), _maxScale);
  form->addRow(tr("Num. scales"), _numScales);
  form->addRow(tr("Wavelet"), _wavelet);
  _param1Label = new QLabel(tr("alpha"));
  form->addRow(_param1Label, _param1);

  QDialogButtonBox *bb = new QDialogButtonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addLayout(form);
  layout->addWidget(bb);

  setLayout(layout);

  connect(bb, SIGNAL(accepted()), this, SLOT(accept()));
  connect(bb, SIGNAL(rejected()), this, SLOT(reject()));
  connect(_wavelet, SIGNAL(currentIndexChanged(int)),
          this, SLOT(waveletSelected(int)));
}

TransformDialog::Wavelet
TransformDialog::wavelet() const {
  return Wavelet(_wavelet->currentData().toInt());
}

TransformedItem::Scaling
TransformDialog::scaling() const {
  return TransformedItem::Scaling(_scaling->currentData().toUInt());
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

double
TransformDialog::param1() const {
  return _param1->text().toDouble();
}

void
TransformDialog::waveletSelected(int idx) {
  if (0 == idx) { // Morlet
    _param1Label->setText(tr("df/f"));
    _param1->setText("0.5");
  } else if (1 == idx) { // Cauchy
    _param1Label->setText(tr("alpha"));
    _param1->setText("20");
  }
}
