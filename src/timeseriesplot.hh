#ifndef TIMESERIESPLOT_HH
#define TIMESERIESPLOT_HH

#include "qcustomplot.hh"

class TimeseriesItem;


class TimeseriesPlot: public QCustomPlot
{
  Q_OBJECT

public:
  class Settings {
  public:
    Settings();
    Settings(const Settings &other);
    Settings &operator=(const Settings &other);

    bool showTitle() const;
    void setShowTitle(bool show);

  protected:
    bool _showTitle;
  };

public:
  TimeseriesPlot(TimeseriesItem *item, const Settings &settings = Settings(), QWidget *parent=0);

  const Settings &settings() const;
  void applySettings(const Settings &settings);

public slots:
  void crop(bool crop);

signals:
  void cropped(double x1, double x2);

protected:
  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);

protected:
  TimeseriesItem *_item;
  QCPTextElement *_title;
  Settings _settings;
  bool _cropping;
  bool _dragging;
  QCPItemRect *_selection;
};

#endif // TIMESERIESPLOT_HH
