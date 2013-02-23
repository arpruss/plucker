#include <UnicodeMatcher.h>
#include <UnicodePattern.h>

const int UnicodeMatcher::MATCH_ENTIRE_STRING = 0x01;

/*
  Detailed documentation is provided in this class' header file

  @author   Jeffery Stuart
  @since    November 2004
  @version  0.01a
*/

UnicodeMatcher::UnicodeMatcher(UnicodePattern * pattern, const std::wstring & text)
{
  pat = pattern;
  str = text;
  gc = pattern->groupCount;
  ncgc = pattern->nonCapGroupCount;
  flags = 0;
  matchedSomething = false;
  starts        = new int[gc + ncgc];
  ends          = new int[gc + ncgc];
  groups        = new int[gc + ncgc];
  groupPos      = new int[gc + ncgc];
  groupIndeces  = new int[gc + ncgc];
  starts        = starts        + ncgc;
  ends          = ends          + ncgc;
  groups        = groups        + ncgc;
  groupPos      = groupPos      + ncgc;
  groupIndeces  = groupIndeces  + ncgc;
  for (int i = 0; i < gc; ++i) starts[i] = ends[i] = 0;
}
UnicodeMatcher::~UnicodeMatcher()
{
  delete [] (starts       - ncgc);
  delete [] (ends         - ncgc);
  delete [] (groups       - ncgc);
  delete [] (groupIndeces - ncgc);
  delete [] (groupPos     - ncgc);
}
void UnicodeMatcher::clearGroups()
{
  int i;
  lm = 0;
  for (i = 0; i < gc; ++i)    groups[i] = starts[i] = ends[i] = -1;
  for (i = 1; i <= ncgc; ++i) groups[0 - i] = -1;
}
std::wstring UnicodeMatcher::replaceWithGroups(const std::wstring & str)
{
  std::wstring ret = L"";
  unsigned int i;

  for (i = 0; i < str.size(); ++i)
  {
    if (str[i] == (wchar_t)'\\')
    {
      if      (i == str.length() - 1)                   ret += L"\\";
      else if (str[i + 1] <= (wchar_t)'0' || str[i + 1] >= (wchar_t)'9')
      {
        ++i;
        ret += str.substr(i, 1);
      }
      else
      {
        int gn = 0;
        ++i;
        while (i < str.length() && str[i] >= (wchar_t)'0' && str[i] <= (wchar_t)'9')
        {
          gn = gn * 10 + (str[i] - (wchar_t)'0');
          i++;
        }
        ret += getGroup(gn);
      }
    }
    else ret += str.substr(i, 1);
  }


  return ret;
}
unsigned long UnicodeMatcher::getFlags() const
{
  return flags;
}
std::wstring UnicodeMatcher::getText() const
{
  return str;
}

bool UnicodeMatcher::matches()
{
  flags = MATCH_ENTIRE_STRING;
  matchedSomething = false;
  clearGroups();
  lm = 0;
  return pat->head->match(str, this, 0) == (int)str.size();
}
bool UnicodeMatcher::findFirstMatch()
{
  starts[0] = 0;
  flags = 0;
  clearGroups();
  start = 0;
  lm = 0;
  ends[0] = pat->head->match(str, this, 0);
  if (ends[0] >= 0)
  {
    matchedSomething = true;
    return 1;
  }
  return 0;
}
bool UnicodeMatcher::findNextMatch()
{
  int s = starts[0], e = ends[0];

  if (!matchedSomething) return findFirstMatch();
  if (s == e) ++e;
  flags = 0;
  clearGroups();

  starts[0] = e;
  if (e >= (int)str.size()) return 0;
  start = e;
  lm = e;
  ends[0] = pat->head->match(str, this, e);
  return ends[0] >= 0;
}
std::vector<std::wstring> UnicodeMatcher::findAll()
{
  std::vector<std::wstring> ret;
  reset();
  while (findNextMatch())
  {
    ret.push_back(getGroup());
  }
  return ret;
}

void UnicodeMatcher::reset()
{
  lm = 0;
  clearGroups();
  matchedSomething = false;
}

int UnicodeMatcher::getStartingIndex(const int groupNum) const
{
  if (groupNum < 0 || groupNum >= gc) return -1;
  return starts[groupNum];
}
int UnicodeMatcher::getEndingIndex(const int groupNum) const
{
  if (groupNum < 0 || groupNum >= gc) return -1;
  return ends[groupNum];
}
std::wstring UnicodeMatcher::getGroup(const int groupNum) const
{
  if (groupNum < 0 || groupNum >= gc) return L"";
  return str.substr(starts[groupNum], ends[groupNum] - starts[groupNum]);
}
std::vector<std::wstring> UnicodeMatcher::getGroups(const bool includeGroupZero) const
{
  int i, start = (includeGroupZero ? 0 : 1);
  std::vector<std::wstring> ret;

  for (i = start; i < gc; ++i) ret.push_back(getGroup(i));
  return ret;
}

