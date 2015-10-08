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

class BindingsH
  include ERB::Util

  def initialize(packages)
    @packages = packages
  end

  def render()
    @superclasses = find_superclasses @packages
    @includes = find_includes @superclasses
    ERB.new(template).result(binding)
  end

  def template
  %{/*<%=gpl_license%>*/
#ifndef BINDTYPES_H
#define BINDTYPES_H

#include "squirrel.h"
<% @includes.each do |include,arg| %>
#include "<%=include%>.h"<% end %>

namespace binding
{
    // type conversion methods<%@superclasses.each do|superclass,subclasses|%>
    <%=superclass%> *get<%=superclass%>(SQUserPointer ptr, SQUserPointer typeTag);<%end%>
}

#endif // BINDTYPES_H}
  end
end


types = BindingsH.new(load_models)
filePath = "../../bindings/bindtypes.h";
write_if_changed(filePath, types.render())
