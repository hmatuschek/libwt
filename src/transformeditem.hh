#ifndef TRANSFORMEDITEM_HH
#define TRANSFORMEDITEM_HH

#include "item.hh"
#include "api.hh"
#include <Eigen/Eigen>
#include <QWidget>
#include <QComboBox>
#include "transformedplot.hh"
#include "application.hh"


class TransformedItem: public Item
{
  Q_OBJECT

public:
  typedef enum {
    LINEAR, DYADIC, DECADIC
  } Scaling;

public:
  TransformedItem(const wt::Wavelet &wavelet, double Fs, double t0,
                  const Eigen::Ref<const Eigen::VectorXd> &scales,
                  Scaling scaling, const Eigen::Ref<const Eigen::MatrixXcd> &data,
                  const QString &label="transformed", QObject *parent=0);
  virtual ~TransformedItem();

  inline wt::Wavelet wavelet() const { return _wavelet; }
  inline double t0() const { return _t0; }
  inline double Fs() const { return _Fs; }
  inline const Eigen::VectorXd &scales() const { return _scales; }
  inline Scaling scaling() const { return _scaling; }
  inline const Eigen::MatrixXcd &data() const { return _data; }

  QWidget *view();

protected:
  wt::Wavelet _wavelet;
  double _Fs;
  double _t0;
  Eigen::VectorXd _scales;
  Scaling _scaling;
  Eigen::MatrixXcd _data;
};


class TransformedItemView: public QWidget
{
  Q_OBJECT

public:
  TransformedItemView(TransformedItem *item, QWidget *parent=0);

protected slots:
  void doSynthesis();
  void doProjection();
  void truncate();
  void config();
  void savePlot();
  void saveData();
  void delItem();
  void cropped(const Polygon &poly);

protected:
  TransformedItem *_item;
  TransformedPlot *_plot;
  QAction *_cropAction;
};


class TransformedItemViewConfigDialog: public QDialog
{
  Q_OBJECT

public:
  TransformedItemViewConfigDialog(Application *app, const QString &label,
                                  const TransformedPlot::Settings &plotSettings, QWidget *parent=0);

  QString label() const;
  TransformedPlot::Settings plotSettings() const;

public slots:
  virtual void accept();

protected:
  Application *_application;
  QCheckBox *_showTitle;
  QComboBox *_showModulus;
  QCheckBox *_showVoice;
  QCheckBox *_showZoom;
  QCheckBox *_showWavelet;
  QLineEdit *_label;
  QString _oldLabel;
};


#endif // TRANSFORMEDITEM_HH
