#include "town.hpp"

static quint32    MagicNumber    = 0x00000002; // 0x00000002->CTown
static quint32    Version        = 0x00000000;
static TCoordType enlargePercent = static_cast<TCoordType>(0.02);

void CTown::setPaths (TPaths const & paths, bool boundingBox)
{
  m_d->m_paths = paths;
  if (boundingBox)
  {
    m_d->m_area     = 0;
    TCoordType coef = static_cast<TCoordType>(0.5);
    m_d->m_aabb.init ();
    for (TPath const & path : qAsConst (m_d->m_paths))
    {
      int vertexCount = path.size ();
      for (int i = 0; i < vertexCount; ++i)
      {
        TCoordType x = path[i].x ();
        TCoordType y = path[i].y ();
        m_d->m_aabb.add (x, y);
        int i1       = i == vertexCount - 1 ? 0 : i + 1;
        m_d->m_area += coef * (path[i1].y () + y) * (path[i1].x () - x);   //S = [ (B+b) x h ] : 2
      }

      coef = -coef;
    }

    m_d->m_aabb.enlarge (enlargePercent);
    TCoordType const epsilon = static_cast<TCoordType>(1.0e-10);
    if (m_d->m_area < epsilon)
    {
      m_d->m_area = epsilon;
    }
  }
}

void CTown::appendPaths (TPaths const & paths)
{
  TCoordType coef = static_cast<TCoordType>(0.5);
  m_d->m_paths.append (paths);
  for (TPath const & path : qAsConst (paths))
  {
    int vertexCount = path.size ();
    for (int i = 0; i < vertexCount; ++i)
    {
      TCoordType x = path[i].x ();
      TCoordType y = path[i].y ();
      m_d->m_aabb.add (x, y);
      int i1       = i == vertexCount - 1 ? 0 : i + 1;
      m_d->m_area += coef * (path[i1].y () + y) * (path[i1].x () - x);   //S = [ (B+b) x h ] : 2
    }

    coef = -coef;
  }

  m_d->m_aabb.enlarge (enlargePercent);
  TCoordType const epsilon = static_cast<TCoordType>(1.0e-10);
  if (m_d->m_area < epsilon)
  {
    m_d->m_area = epsilon;
  }
}

void CTown::updateCentroid ()
{
  int cp = 0;
  for (TPath const & path : qAsConst (m_d->m_paths))
  {
    cp += path.size ();
    for (TGeoCoord const & p : path)
    {
      m_d->m_centroid += p;
    }
  }

  m_d->m_centroid /= cp;
}

QDataStream& operator >> (QDataStream& in, CTown::TPath& path)
{
  quint32 count; in >> count;
  path.reserve (count);
  for (quint32 i = 0; i < count; ++i)
  {
    TGeoCoord vertex; in >> vertex;
    path.append (vertex);
  }

  return in;
}

QDataStream& operator >> (QDataStream& in, CTown::TPaths& paths)
{
  quint32 count; in >> count;
  paths.reserve (count);
  for (quint32 i = 0; i < count; ++i)
  {
    CTown::TPath path; in >> path;
    paths.append (path);
  }

  return in;
}

QDataStream& operator >> (QDataStream& in, CTown& town)
{
  quint32 magicNumber; in >> magicNumber;
  if (magicNumber == MagicNumber)
  {
    quint32 version; in >> version;
    if (version == Version)
    {
      QString       code;   in >> code;   town.setCode (CTown::townCode (code));
      QString       name;   in >> name;   town.setName (name);
      QString       region; in >> region; town.setRegion (CTown::regionCode (region));
      CTown::TPaths paths;  in >> paths;  town.setPaths (paths, false);
      TCoordType    area;   in >> area;   town.setArea (area);
      CAabb         bb;     in >> bb;     town.setAabb (bb);
      town.updateCentroid (); // Not in file.
    }
  }

  return in;
}
