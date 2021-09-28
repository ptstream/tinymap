#ifndef MAPSHAPEDATA_HPP
#define MAPSHAPEDATA_HPP

#include <QDialog>

namespace Ui {
  class CMapShapeData;
}

class CTown;

class CMapShapeData : public QDialog
{
  Q_OBJECT

public:
  explicit CMapShapeData (CTown const & town, QWidget *parent = nullptr);
  ~CMapShapeData ();

private:
  Ui::CMapShapeData* ui;
};

#endif // MAPSHAPEDATA_HPP
