/*
 *  Copyright (C) 2012-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "MusicDbUrl.h"

#include "filesystem/MusicDatabaseDirectory.h"
#include "playlists/SmartPlayList.h"
#include "utils/StringUtils.h"
#include "utils/Variant.h"

using namespace KODI;
using namespace XFILE;
using namespace XFILE::MUSICDATABASEDIRECTORY;

CMusicDbUrl::CMusicDbUrl()
  : CDbUrl()
{ }

CMusicDbUrl::~CMusicDbUrl() = default;

bool CMusicDbUrl::parse()
{
  // the URL must start with musicdb://
  if (!m_url.IsProtocol("musicdb") || m_url.GetFileName().empty())
    return false;

  std::string path = m_url.Get();

  // Parse path for directory node types and query params
  NODE_TYPE dirType;
  NODE_TYPE childType;
  CQueryParams queryParams;
  if (!CMusicDatabaseDirectory::GetDirectoryNodeInfo(path, dirType, childType, queryParams))
    return false;

  switch (dirType)
  {
    case NODE_TYPE_ARTIST:
      m_type = "artists";
      break;

    case NODE_TYPE_ALBUM:
    case NODE_TYPE_ALBUM_RECENTLY_ADDED:
    case NODE_TYPE_ALBUM_RECENTLY_PLAYED:
    case NODE_TYPE_ALBUM_TOP100:
      m_type = "albums";
      break;

    case NODE_TYPE_DISC:
      m_type = "discs";
      break;

    case NODE_TYPE_ALBUM_RECENTLY_ADDED_SONGS:
    case NODE_TYPE_ALBUM_RECENTLY_PLAYED_SONGS:
    case NODE_TYPE_ALBUM_TOP100_SONGS:
    case NODE_TYPE_SONG:
    case NODE_TYPE_SONG_TOP100:
    case NODE_TYPE_SINGLES:
      m_type = "songs";
      break;

    default:
      break;
  }

  switch (childType)
  {
    case NODE_TYPE_ARTIST:
      m_type = "artists";
      break;

    case NODE_TYPE_ALBUM:
    case NODE_TYPE_ALBUM_RECENTLY_ADDED:
    case NODE_TYPE_ALBUM_RECENTLY_PLAYED:
    case NODE_TYPE_ALBUM_TOP100:
      m_type = "albums";
      break;

    case NODE_TYPE_DISC:
      m_type = "discs";
      break;

    case NODE_TYPE_SONG:
    case NODE_TYPE_ALBUM_RECENTLY_ADDED_SONGS:
    case NODE_TYPE_ALBUM_RECENTLY_PLAYED_SONGS:
    case NODE_TYPE_ALBUM_TOP100_SONGS:
    case NODE_TYPE_SONG_TOP100:
    case NODE_TYPE_SINGLES:
      m_type = "songs";
      break;

    case NODE_TYPE_GENRE:
      m_type = "genres";
      break;

    case NODE_TYPE_SOURCE:
      m_type = "sources";
      break;

    case NODE_TYPE_ROLE:
      m_type = "roles";
      break;

    case NODE_TYPE_YEAR:
      m_type = "years";
      break;

    case NODE_TYPE_TOP100:
      m_type = "top100";
      break;

    case NODE_TYPE_ROOT:
    case NODE_TYPE_OVERVIEW:
    default:
      return false;
  }

  if (m_type.empty())
    return false;

  // retrieve and parse all options
  AddOptions(m_url.GetOptions());

  // add options based on the node type
  if (dirType == NODE_TYPE_SINGLES || childType == NODE_TYPE_SINGLES)
    AddOption("singles", true);

  // add options based on the QueryParams
  if (queryParams.GetArtistId() != -1)
    AddOption("artistid", (int)queryParams.GetArtistId());
  if (queryParams.GetAlbumId() != -1)
    AddOption("albumid", (int)queryParams.GetAlbumId());
  if (queryParams.GetGenreId() != -1)
    AddOption("genreid", (int)queryParams.GetGenreId());
  if (queryParams.GetSongId() != -1)
    AddOption("songid", (int)queryParams.GetSongId());
  if (queryParams.GetYear() != -1)
    AddOption("year", (int)queryParams.GetYear());

  // Decode legacy use of "musicdb://compilations/" path for filtered albums
  if (m_url.GetFileName() == "compilations/")
    AddOption("compilation", true);

  return true;
}

bool CMusicDbUrl::validateOption(const std::string &key, const CVariant &value)
{
  if (!CDbUrl::validateOption(key, value))
    return false;

  // if the value is empty it will remove the option which is ok
  // otherwise we only care about the "filter" option here
  if (value.empty() || !StringUtils::EqualsNoCase(key, "filter"))
    return true;

  if (!value.isString())
    return false;

  PLAYLIST::CSmartPlaylist xspFilter;
  if (!xspFilter.LoadFromJson(value.asString()))
    return false;

  // check if the filter playlist matches the item type
  return xspFilter.GetType() == m_type;
}
