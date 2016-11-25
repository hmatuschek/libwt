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

protected:
  TimeseriesItem(double Fs, double t0=0, const QString &label="timeseries", QObject *parent=0);

public:
  virtual ~TimeseriesItem();

  inline double t0() const { return _t0; }
  inline double Fs() const { return _Fs; }
  virtual size_t size() const = 0;

  QWidget *view();

protected:
  double _Fs;
  double _t0;
};


class RealTimeseriesItem: public TimeseriesItem
{
  Q_OBJECT

public:
  RealTimeseriesItem(const Eigen::Ref<const Eigen::VectorXd> &data, double Fs, double t0=0,
                     const QString &label="timeseries", QObject *parent=0);
  virtual ~RealTimeseriesItem();

  inline const Eigen::VectorXd &data() const { return _data; }
  inline Eigen::VectorXd &data() { return _data; }
  virtual size_t size() const;

protected:
  Eigen::VectorXd _data;
};


class ComplexTimeseriesItem: public TimeseriesItem
{
  Q_OBJECT

public:
  ComplexTimeseriesItem(const Eigen::Ref<const Eigen::VectorXcd> &data, double Fs, double t0=0,
                        const QString &label="timeseries", QObject *parent=0);
  virtual ~ComplexTimeseriesItem();

  inline const Eigen::VectorXcd &data() const { return _data; }
  inline Eigen::VectorXcd &data() { return _data; }
  virtual size_t size() const;

protected:
  Eigen::VectorXcd _data;
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
