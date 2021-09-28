#include "towns.hpp"
#include <QElapsedTimer>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QDebug>

static quint32 MagicNumber = 0x00001002; // 0x00000002->CTown; 0x00001000->set of towns
static quint32 Version     = 0x00000000;

CTowns::CTowns (QString const & fileName)
{
  load (fileName);
}

// Towns from bibary file generated from
// https://geoservices.ign.fr/documentation/diffusion/telechargement-donnees-libres.html#admin-express (communes.geojson)
void CTowns::load (QString const & fileName)
{
  if (!fileName.isEmpty ())
  {
    QFile file (fileName);
    if (file.open (QIODevice::ReadOnly))
    {
      QDataStream in (&file);
      in >> *this;
      file.close ();
    }
  }
}

QDataStream& operator >> (QDataStream& in, CTowns& towns)
{
  quint32 magicNumber; in >> magicNumber;
  if (magicNumber == MagicNumber)
  {
    quint32 version; in >> version;
    if (version == Version)
    {
      quint32 count; in >> count;
      towns.reserve (count);
      for (quint32 i = 0; i < count; ++i)
      {
        CTown town; in >> town;
        towns.insert (town.code (), town);
      }
    }
  }

  return in;
}
