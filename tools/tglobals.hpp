#ifndef TGLOBALS_HPP
#define TGLOBALS_HPP

#include "../tools/vector.hpp"

/*! TOOLS_VERSION is (major << 16) + (minor << 8) + patch. */
#define TOOLS_VERSION 0x010000

#ifdef DOUBLECOORDTYPE
using TCoordType = double;
#else
using TCoordType = float;
#endif

/*! A TGeoCoord is defined by longitude, latitude.
 * The coordinate values should be specified using the WGS84 datum.
 */
using TGeoCoord = CVector<TCoordType, 2>; // x = longitude, y = latitude.

/*! Approximative distance of two points separated by 1 degree on the earth. */
TCoordType const metersPerDegree = static_cast<TCoordType>(111320.0);

/*! Mean radius of the earth. */
TCoordType const earthMeanRadius = static_cast<TCoordType>(6371007.2);

/*! \brief Returns degrees converted in radians */
template<typename T>
inline T dgToRd (T x)
{
  return x * static_cast<T>(M_PI) / 180;
}

/*! \brief Returns radians converted in degrees */
template<typename T>
inline T rdToDg (T x)
{
  return x * 180 / static_cast<T>(M_PI);
}

/*! \brief Returns the integer power of 2 of an integer. */
template<typename T>
T powerOf2 (T x)
{
  return (static_cast<T>(1) << x);
}

#endif // TLOBALS_HPP
