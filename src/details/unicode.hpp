#pragma once

namespace rapidfuzz {
namespace Unicode {

/*
 * checks whether unicode characters have the bidirectional
 * type 'WS', 'B' or 'S' or the category 'Zs'
 */
template <typename T>
bool is_space(const T ch)
{
  switch (ch) {
  case 0x0009:
  case 0x000A:
  case 0x000B:
  case 0x000C:
  case 0x000D:
  case 0x001C:
  case 0x001D:
  case 0x001E:
  case 0x001F:
  case 0x0020:
  case 0x0085:
  case 0x00A0:
  case 0x1680:
  case 0x2000:
  case 0x2001:
  case 0x2002:
  case 0x2003:
  case 0x2004:
  case 0x2005:
  case 0x2006:
  case 0x2007:
  case 0x2008:
  case 0x2009:
  case 0x200A:
  case 0x2028:
  case 0x2029:
  case 0x202F:
  case 0x205F:
  case 0x3000:
    return true;
  }
  return false;
}

} // namespace Unicode
} // namespace rapidfuzz
