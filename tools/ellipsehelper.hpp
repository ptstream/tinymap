#ifndef MATH_HPP
#define MATH_HPP

// https://www.geometrictools.com/Documentation/DistancePointEllipseEllipsoid.pdf
#include "tglobals.hpp"

class CEllipseHelper
{
public:
  /*! Contructor. */
  CEllipseHelper (int maxIterations = 100) : m_maxIterations (maxIterations) {}

  /*! \brief Sets the max number of iterations to get root. */
  void setMaxIterations (int maxIterations) { m_maxIterations = maxIterations; }

  /*! \brief Returns the max number of iterations to get root. */
  int iterationCount () const { return m_iterationsCount; }

  /*! \brief Returns the distance from point to an 2D ellipse.
   *  \param e0: Major axis.
   *  \param e1: Minor axis.
   *  \param p: The point. It is also the corresponding nearest point on the ellipse.
   *  \return the distance between the nearest point on the ellipse and the given point.
   */
  TCoordType distancePointEllipse (TCoordType e0, TCoordType e1, TGeoCoord& p);

private:
  TCoordType getRoot (TCoordType r0, TCoordType z0, TCoordType z1, TCoordType g);
  TCoordType distancePointQuarterEllipse (TCoordType e0, TCoordType e1, TGeoCoord& p);

private:
  int m_maxIterations, m_iterationsCount;
};

#endif // MATH_HPP
