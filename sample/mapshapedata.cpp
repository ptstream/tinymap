#include "mapshapedata.hpp"
#include "ui_mapshapedata.h"
#include "../town/town.hpp"

CMapShapeData::CMapShapeData (CTown const & town, QWidget *parent) : QDialog (parent), ui (new Ui::CMapShapeData)
{
  setWindowFlag (Qt::WindowContextHelpButtonHint, false);
  ui->setupUi (this);
  ui->m_name->setText (town.name ());
  ui->m_code->setText (QString::number (town.code (), 16));
  ui->m_region->setText (QString::number (town.region ()));
}

CMapShapeData::~CMapShapeData ()
{
  delete ui;
}
