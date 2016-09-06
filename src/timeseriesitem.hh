#ifndef TIMESERIESITEM_HH
#define TIMESERIESITEM_HH

#include "item.hh"
#include <Eigen/Eigen>
#include <QWidget>
#include <QLineEdit>

#include "timeseriesplot.hh"
#include "application.hh"

class TimeseriesItem: public Item
{
  Q_OBJECT

public:
  TimeseriesItem(const Eigen::Ref<const Eigen::VectorXd> &data, double Fs, const QString &label="timeseries", QObject *parent=0);
  virtual ~TimeseriesItem();

  inline double Fs() const { return _Fs; }
  inline const Eigen::VectorXd &data() const { return _data; }
  inline Eigen::VectorXd &data() { return _data; }

  QWidget *view();

protected:
  Eigen::VectorXd _data;
  double _Fs;
};


class TimeseriesItemView: public QWidget
{
  Q_OBJECT

public:
  TimeseriesItemView(TimeseriesItem *item, QWidget *parent=0);

protected slots:
  void doTransform();
  void cropped(double x1, double x2);
  void detrend();
  void config();
  void savePlot();
  void saveData();
  void delItem();

protected:
  TimeseriesItem *_item;
  TimeseriesPlot *_plot;
  QAction *_cropAction;
};


class TimeseriesItemViewConfigDialog: public QDialog
{
  Q_OBJECT

public:
  TimeseriesItemViewConfigDialog(Application *app, const QString &label,
                                 const TimeseriesPlot::Settings &plotSettings, QWidget *parent=0);

  QString label() const;
  TimeseriesPlot::Settings plotSettings() const;

public slots:
  virtual void accept();

protected:
  Application *_application;
  QLineEdit *_label;
  QString _oldLabel;
};


#endif // TIMESERIESITEM_HH
