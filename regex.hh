/**
 *  Copyright: (c) 2014 François Lozes <emvivre@urdn.com.ua>
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the Do What The Fuck You Want To
 *  Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 */

/*
  ===========================================================================

  Copyright (C) 2013 Emvivre

  This file is part of REGEXP.

  REGEXP is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  REGEXP is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with REGEXP.  If not, see <http://www.gnu.org/licenses/>.

  ===========================================================================
*/

#include <vector>
#include <string>

class Regex
{
public:
	class ExceptionRegexpCompilationError {};
	static std::vector<std::string> search(std::string pattern, std::string buffer);
};
