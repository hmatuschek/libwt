#ifndef TRANSFORMDIALOG_HH
#define TRANSFORMDIALOG_HH

#include <QDialog>
#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>

class TransformDialog : public QDialog
{
  Q_OBJECT

public:
  typedef enum {
    MORLET_WAVELET,
    CAUCHY_WAVELET
  } Wavelet;

  typedef enum {
    LINEAR, DYADIC, DECADIC
  } Scaling;

public:
  explicit TransformDialog(size_t Ns, double Fs, QWidget *parent = 0);

  Wavelet wavelet() const;
  Scaling scaling() const;
  double minScale() const;
  double maxScale() const;
  size_t numScales() const;

protected:
  QComboBox *_wavelet;
  QComboBox *_scaling;
  QLineEdit *_minScale;
  QLineEdit *_maxScale;
  QSpinBox  *_numScales;
};

#endif // TRANSFORMDIALOG_HH
