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
    ERB.new(template).result(binding)
  end

  def template
  %{/*<%=gpl_license%>*/
#ifndef BINDINGS_H
#define BINDINGS_H

#include "squirrel.h"

namespace binding
{
    // package binding methods<%@packages.each do|index, package|%>
    void bind<%=package['name']%>(HSQUIRRELVM vm);<%end%>
}

#endif // BINDINGS_H}
  end
end


types = BindingsH.new(load_models)
filePath = "../../bindings/bindings.h";
write_if_changed(filePath, types.render())

