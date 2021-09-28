#ifndef TOWNS_HPP
#define TOWNS_HPP

#include "town.hpp"
#include <QHash>

/*! This is the towns container. */
class CTowns : public QHash<int, CTown>
{
public:
  /*! Constructor from file. */
  CTowns (QString const & fileName = QString ());

  /*! Constructor from set of towns. */
  CTowns (QHash<int, CTown> const & towns) { *static_cast<QHash<int, CTown>*>(this) = towns; }

  /*! Destructor. */
  ~CTowns () = default;

  /*! Loads the container from a file (see >> operator). */
  void load (QString const & fileName);
};

/*! \brief Reads towns file.
 *
 *  The file contains:
 *  magicNumber; // 0x00001002 (0x00000002->CTown and 0x00001000->set of towns)
 *  version;     // 0
 *  count;       // Number of towns
 *  town
 *  town
 *  ...
 *  town
 */
QDataStream& operator >> (QDataStream& in, CTowns& towns);

#endif // TOWNS_HPP
