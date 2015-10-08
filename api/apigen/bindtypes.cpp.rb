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

class TypesCpp
  include ERB::Util
#  attr_accessor :package, :template
  def initialize(superclasses, superpackages)
    @superclasses = superclasses
    @superpackages = superpackages
  end

  def render
    ERB.new(template).result(binding)
  end  

  def template
  %{/*<%=gpl_license%>*/

#include "bindtypes.h"
<%@superpackages.each do|name,arg|%>
#include "bind<%=name.downcase%>.h"<%end%>

namespace binding
{
<%@superclasses.each do|superclass,subclasses|%>
    <%=superclass%> *get<%=superclass%>(SQUserPointer ptr, SQUserPointer typeTag) {<%subclasses.each do|type|%>
        if(typeTag ==  &<%=type['package']%><%=type['name']%>Object) {
            return static_cast<<%=cppname type%>*>(ptr);
        }<%end%>        
        return 0;
    }
    <%end%>
}}
  end
end

superclasses = find_superclasses(load_models)

# find packages with superclasses
superpackages = Hash.new
superclasses.each do|superclass,subclasses|
    subclasses.each do|type|
        superpackages[type['package']] = true
    end
end

types = TypesCpp.new(superclasses, superpackages)
filePath = "../../bindings/bindtypes.cpp";
write_if_changed(filePath, types.render())
