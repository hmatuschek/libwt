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
    bool showModulus() const;
    void setShowModulus(bool show);

    bool showRepKern() const;
    void setShowRepKern(bool show);
    bool showVoice() const;
    void setShowVoice(bool show);
    bool showZoom() const;
    void setShowZoom(bool show);
    bool showWavelet() const;
    void setShowWavelet(bool show);

    bool showLeftPane() const;
    bool showBottomPane() const;

  protected:
    bool _showTitle;
    bool _showModulus;
    bool _showRepKern;
    bool _showVoice;
    bool _showZoom;
    bool _showWavelet;
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
  QCPColorMap *_colorMap;
  Settings _settings;
  QCPTextElement *_title;
  QCPCurve *_valid;
  bool _cropping;
  QPoint _start;
  QCPCurve *_curve;
  Polygon _polygon;
  QCPColorMap *_rkOverlay;
  QCPAxisRect *_leftPaneAxis;
  QCPAxisRect *_bottomPaneAxis;
};




#endif // TRANSFORMEDPLOT_HH
