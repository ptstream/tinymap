#ifndef OSMTILEADAPTER_HPP
#define OSMTILEADAPTER_HPP

#include "tileadapter.hpp"

/*! \brief The COsmTileAdapter class used to manage tiles from OSM tile servers. */
class COsmTileAdapter : public CTileAdapter
{
public:
  /*! Contructor.
   *  \param apiKey: If you have an API key. See https://www.thunderforest.com/ to get an API key.
   */
  COsmTileAdapter (QString const & apiKey = QString ());
};

#endif // OSMTILEADAPTER_HPP
