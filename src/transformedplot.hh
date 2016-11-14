#ifndef TRANSFORMEDPLOT_HH
#define TRANSFORMEDPLOT_HH

#include "qcustomplot.hh"
#include "polygon.hh"

class TransformedItem;


class TransformedPlot: public QCustomPlot
{
  Q_OBJECT

public:
  class Settings
  {
  public:
    Settings();
    Settings(const Settings &other);
    Settings &operator= (const Settings &other);

    bool showTitle() const;
    void setShowTitle(bool show);

  protected:
    bool _showTitle;
  };

public:
  TransformedPlot(TransformedItem *item, const Settings &settings=Settings(), QWidget *parent=0);

  const Settings &settings() const;
  void applySettings(const Settings &settings);

public slots:
  void crop(bool crop);

signals:
  void cropped(const Polygon &polygon);

protected:
  void mouseReleaseEvent(QMouseEvent *event);

protected:
  TransformedItem *_item;
  Settings _settings;
  QCPPlotTitle *_title;
  QCPCurve *_valid;
  bool _cropping;
  QPoint _start;
  QCPCurve *_curve;
  Polygon _polygon;
};




#endif // TRANSFORMEDPLOT_HH