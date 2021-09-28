#ifndef MAPBOXTILEADAPTER_HPP
#define MAPBOXTILEADAPTER_HPP

#include "tileadapter.hpp"

/*! \brief The CMapboxTileAdapter class used to manage tiles from Map tile servers.
 *  \remark This object fails.
 */
class CMapboxTileAdapter : public CTileAdapter
{
public:
  /*! Contructor. */
  CMapboxTileAdapter (QString const & apiKey = "mandatory");
};

#endif // MAPBOXTILEADAPTER_HPP
