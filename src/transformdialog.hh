#ifndef TRANSFORMDIALOG_HH
#define TRANSFORMDIALOG_HH

#include <QDialog>
#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>

#include "transformeditem.hh"

class TransformDialog : public QDialog
{
  Q_OBJECT

public:
  typedef enum {
    MORLET_WAVELET,
    CAUCHY_WAVELET,
    REGMORLET_WAVELET
  } Wavelet;

public:
  explicit TransformDialog(size_t Ns, double Fs, QWidget *parent = 0);

  Wavelet wavelet() const;
  TransformedItem::Scaling scaling() const;
  double minScale() const;
  double maxScale() const;
  size_t numScales() const;
  double param1() const;

protected slots:
  void waveletSelected(int idx);

protected:
  QComboBox *_scaling;
  QLineEdit *_minScale;
  QLineEdit *_maxScale;
  QSpinBox  *_numScales;
  QComboBox *_wavelet;
  QLabel *_param1Label;
  QLineEdit *_param1;
};

#endif // TRANSFORMDIALOG_HH
