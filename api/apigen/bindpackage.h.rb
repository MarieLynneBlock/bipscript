#
# This file is part of Bipscript.
#
# Bipscript is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Bipscript is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Bipscript.  If not, see <http://www.gnu.org/licenses/>.
#

require 'erb'
require 'yaml'
require './apigen.rb'

class PackageH
  include ERB::Util

  def initialize(package)
    @package = package
  end

  def render()
    ERB.new(template).result(binding)
  end

  def template
  %{/*<%=gpl_license%>*/
#ifndef BIND<%=@package['name'].upcase%>_H
#define BIND<%=@package['name'].upcase%>_H

#include "squirrel.h"

namespace binding
{
// object references to types in this package<% with @package['classes'] do |clazz| %>
extern HSQOBJECT <%=@package['name']%><%=clazz['name']%>Object;<% end %>
}

#endif // BIND<%=@package['name'].upcase%>_H}
  end
end

models = load_models

models.each do|name,data|
  pack = PackageH.new(data)
  filePath = "../../bindings/bind" + data["name"].downcase + ".h";
  write_if_changed(filePath, pack.render())
end
