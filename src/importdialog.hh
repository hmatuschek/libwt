#ifndef IMPORTDIALOG_HH
#define IMPORTDIALOG_HH

#include <QDialog>
#include <QComboBox>
#include <QSpinBox>
#include <QLineEdit>


class Application;


class ImportDialog : public QDialog
{
  Q_OBJECT

public:
  explicit ImportDialog(const QString &filename, size_t cols, Application &app, QWidget *parent = 0);

  bool real() const;
  size_t realColumn() const;
  size_t imagColumn() const;
  double Fs() const;
  QString label() const;

protected slots:
  void _onTypeSelected(int idx);
  void verify();

protected:
  Application &_application;
  QComboBox *_typeSel;
  QSpinBox *_realCol;
  QSpinBox *_imagCol;
  QLineEdit *_Fs;
  QLineEdit *_label;
};

#endif // IMPORTDIALOG_HH
