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

class PackageCpp
  include ERB::Util

  def initialize(package, superclasses)
    @package = package
    @superclasses = superclasses
  end

  def render
    # calculate includes
    @includes = Hash.new
    with @package['methods'] do |method|
      with method['include'] do |include|
        @includes[include] = true
      end
    end
    with @package['classes'] do |clazz|
      with clazz['include'] do |include|
        @includes[include] = true
      end      
    end
    # calculate binding includes
    @bindincludes = Hash.new
    with @package['classes'] do |clazz|
      with clazz['methods'] do |method|
        with method['parameters'] do |param|
            if param['type'].include? "."
                pack = type_package(param['type'])
                if pack != @package['name']
                    @bindincludes["bind" + pack.downcase] = true
                end
            end
        end
      end
    end
    ERB.new(template).result(binding)
  end

  def param_list obj, max
    ret = ""
    with obj['parameters'] do |param, count|
        if count > max
            break
        end
        if not squirrel_type param['type'] and not object_type param['type']
            ret << "*"
        end
        ret << param["name"] + ", "
    end
    ret.chop.chop
  end

  def squirrel_type type
    case type
    when "float"
        "SQFloat"
    when "integer"
        "SQInteger"
    when "string"
        "const SQChar*"
    else
        nil
    end
  end

  def object_type type
    case type
    when "function"
      true
    when "table"
      true
    else
      false
    end
  end

  def type_tag type
    case type
    when "float"
      "OT_FLOAT"
    when "integer"
      "OT_INTEGER"
    when "string"
      "OT_STRING"
    when "function"
      "OT_CLOSURE"
    when "table"
      "OT_TABLE"
    end
  end

  def type_cppname type
    with @package['classes'] do |clazz|
        if clazz['name'] == type_name(type)
            return cppname(clazz)
        end
    end
    # puts "warning: could not resolve type: " << type
    return type_name type
  end

  def methodnames clazz
    ret = Hash.new
    with clazz['methods'] do |method|
      ret[method['name']] = true
    end
    return ret.keys
  end

  def overrides clazz, methodname, ctor
    ret = []
    if ctor
        with clazz['ctor'] do |method|
            ret << method
        end
    else
        methods = clazz ? clazz['methods'] : @package['methods']
        with methods do |method|
          if method['name'] == methodname
            ret << method
          end
        end
    end
    return ret
  end

  def override_index methods
    with methods[0]['parameters'] do |param, count|
        if (param != methods[1]['parameters'][count-1])
            return count
        end
    end
    throw "no override parameter: methods are identical!"
  end

  def optional_parameters method
    opts = 0
    with method['parameters'] do |param|
        if param['optional']
            opts += 1
        elsif opts > 0
            raise "required parameter follows optional: " + param['name']
        end
    end
    return opts
  end

  def type_package type
    if not type.include? "."
      raise type + " is not a qualified type name"
    end
    type.split("\.")[0]
  end

  def type_name type
    if not type.include? "."
      raise type + " is not a qualified type name"
    end
    type.split("\.")[1]
  end

  def get_parameter param, index, dotypecheck, indent = "    "
    ret = ""
    ret << indent << "// get parameter #{index-1} \"#{param['name']}\" as #{param['type']}\n"

    if squirrel_type param['type']
    ret << indent << "#{squirrel_type param['type']} #{param['name']};\n"
    ret << indent << "if (SQ_FAILED(sq_get#{param['type']}(vm, #{index}, &#{param["name"]}))){\n"
    ret << indent << "    return sq_throwerror(vm, \"argument #{index-1} is not of type #{param['type']}\");\n"
    ret << indent << "}\n"

    elsif param['type'] == "table"
    ret << indent << "HSQOBJECT #{param['name']}Obj;\n"
    ret << indent << "if (SQ_FAILED(sq_getstackobj(vm, #{index}, &#{param['name']}Obj))) {\n"
    ret << indent << "    return sq_throwerror(vm, \"argument #{index-1} is not of type table\");\n"
    ret << indent << "}\n"
    ret << indent << "if (sq_gettype(vm, #{index}) != OT_TABLE) {\n"
    ret << indent << "    return sq_throwerror(vm, \"argument #{index-1} is not of type table\");\n"
    ret << indent << "}\n"
    ret << indent << "ScriptHashIterator #{param['name']}(vm, #{param['name']}Obj);\n"

    elsif object_type param['type']
    ret << indent << "HSQOBJECT #{param['name']};\n"
    ret << indent << "sq_getstackobj(vm, #{index}, &#{param['name']});\n"
    ret << indent << "sq_addref(vm, &#{param['name']});\n"

    elsif @superclasses[param['type']]
    ret << indent << %{SQUserPointer #{param['name']}TypeTag, #{param['name']}Ptr = 0;
    if (SQ_FAILED(sq_getinstanceup(vm, #{index}, (SQUserPointer*)&#{param['name']}Ptr, 0))) {
        return sq_throwerror(vm, "argument #{index-1} is not an object of type #{param['type']}");
    }
    sq_gettypetag(vm, #{index}, &#{param['name']}TypeTag);
    #{param['type']} *#{param['name']} = get#{param['type']}(#{param['name']}Ptr, #{param['name']}TypeTag);
    if(#{param['name']} == 0) {
        return sq_throwerror(vm, "argument #{index-1} is not of type #{param['type']}");
    }\n}

    else # api type with no superclass
    ret << indent << "#{type_cppname param['type']} *#{param['name']} = 0;\n"
    ret << indent << "sq_getinstanceup(vm, #{index}, (SQUserPointer*)&#{param['name']}, 0);\n"
    ret << indent << "if(#{param['name']} == 0) {\n"
    ret << indent << "    return sq_throwerror(vm, \"argument #{index-1} is not of type #{param['type']}\");\n"
    ret << indent << "}\n"
    if dotypecheck
    ret << indent << "SQUserPointer #{param['name']}TypeTag;\n"
    ret << indent << "sq_gettypetag(vm, #{index}, &#{param['name']}TypeTag);\n"
    ret << indent << "if(#{param['name']}TypeTag != &#{type_package param['type']}#{type_name param['type']}Object) {\n"
    ret << indent << "    return sq_throwerror(vm, \"argument #{index-1} is not of type #{param['type']}\");\n"
    ret << indent << "}\n"
    end
    end

    return ret;
  end

  def return_type method
    if squirrel_type(method['returns'])
        squirrel_type(method['returns'])
    else
        method['returns'].split('.')[1] + "*"
    end
  end

  def push_return type
    if squirrel_type type
        "sq_push#{type}(vm, ret);"
    else
    %{sq_pushobject(vm, #{type_package type}#{type_name type}Object);
    sq_createinstance(vm, -1);
    sq_remove(vm, -2);
    sq_setinstanceup(vm, -1, ret);
    //sq_setreleasehook(vm, -1, &?);
    }
    end
  end

  def call_method clazz, method, ctor, paramcount, indent
    ret = ""
    ret << indent << "// call the implementation\n"
    ret << indent << "try {\n" << indent << "    "
    if ctor
        ret << "obj = "
    end
    if method['expression']
        ret << method['expression']
    elsif ctor
        ret << "new " << cppname(clazz)
    else
        if method['returns']
            ret << "ret = "
        end
        if clazz
            ret << "obj->"
        else
            ret << @package['name'] << "::"
        end
        ret << "#{method['cppname']?method['cppname']:method['name']}"
    end
    ret << "(" << param_list(method, paramcount) << ");\n"
    ret << indent << "}\n"
    ret << indent << "catch(std::exception const& e) {\n"
    ret << indent << "    return sq_throwerror(vm, e.what());\n"
    ret << indent << "}\n"
  end

  def bind_methodname clazz, methodname
      ret = ""
      indent = ' ' * 4
      ctor = methodname == "ctor"
      # get overrides
      overrides = overrides clazz, methodname, ctor
      if overrides.size > 1
          overindex = override_index(overrides)
          # is the override parameter optional?
          if overrides[0]['parameters'][overindex-1]['optional']
            # create a version of the method with only required params
            optmethod = overrides[0].clone
            optmethod['parameters'] = overrides[0]['parameters'].slice(0, overindex - 1)
            ret << indent << "SQInteger numargs = sq_gettop(vm);\n"
            ret << indent << "// optional overriden parameter not here\n"
            ret << indent << "if(numargs < #{overindex + 1}) {\n"
            ret << indent << bind_method(clazz, optmethod, ctor, overrides, indent) << "\n"
            ret << indent << "}\n"
          end
          ret << indent << "SQObjectType overrideType = sq_gettype(vm, #{overindex + 1});\n"
          ret << indent << "SQUserPointer overrideTypeTag;\n"
          ret << indent << "if(overrideType == OT_INSTANCE) {\n"
          ret << indent * 2 << "sq_gettypetag(vm, #{overindex + 1}, &overrideTypeTag);\n"
          ret << indent << "}\n\n"
      end
      # outer loop for overridden methods
      with overrides do |method, count|
      if overrides.size > 1
          overindex = override_index(overrides)
          overtype = method['parameters'][overindex-1]['type']
          if squirrel_type overtype or object_type overtype
              ret << indent << "if(overrideType == #{type_tag overtype}) {\n"
          else
              ret << indent << "if(overrideType == OT_INSTANCE && overrideTypeTag == "
              ret << "&#{type_package overtype}#{type_name overtype}Object) {\n"
          end
      end
      ret << bind_method(clazz, method, ctor, overrides, indent)
      if overrides.size > 1
          ret << "\n" << indent << "}\n"
      end
      end
      if overrides.size > 1
          ret << indent << "else {\n"
          ret << indent << "    return sq_throwerror(vm, \"argument #{overindex} is not an expected type\");\n"
          ret << indent << "}"
      end
      return ret
  end

  def bind_method clazz, method, ctor, overrides, indent
    ret = ""
    # count number of parameters returned
    opts = optional_parameters(method)
    if opts > 0
        ret << indent << "SQInteger numargs = sq_gettop(vm);\n\n"
    end
    # "this" pointer
    if clazz and not ctor
        ret << indent << "// get \"this\" pointer\n"
        ret << indent << "SQUserPointer userPtr = 0;\n"
        ret << indent << "sq_getinstanceup(vm, 1, &userPtr, 0);\n"
        ret << indent << cppname(clazz) << " *obj = "
        ret << "static_cast<" << cppname(clazz) << "*>(userPtr);\n\n"
    end

    # required parameters
    with method['parameters'] do |param, counter|
        if not param['optional']
            dotypecheck = overrides.size == 1 or counter != 1
            ret << get_parameter(param, counter + 1, dotypecheck, indent) << "\n"
        end
    end
    # define return value
    if ctor
        ret << indent << cppname(clazz) << " *obj;\n"
    elsif method['returns']
        ret << indent << "// return value\n"
        ret << indent << return_type(method) << " ret;\n"
    end
    # optional parameters
    totalparams = method['parameters'] ? method['parameters'].length : 0
    for i in totalparams-opts+1..totalparams
        ret << indent << "// #{i} parameters passed in\n"
        ret << indent
        if i != totalparams-opts+1 then ret << "else " end
        ret << "if(numargs == #{i+1}) {\n\n"
        with method['parameters'] do |param, counter|
            if param['optional'] and counter <= i
                ret << get_parameter(param, counter + 1, true, indent + "    ") << "\n"
            end
        end
        ret << call_method(clazz, method, ctor, i, indent + "    ")
        ret << indent << "}\n\n"
    end
    # call method with required params
    if opts > 0
        req = totalparams - opts
        ret << indent << "else {\n"
        ret << call_method(clazz, method, ctor, req, indent + "    ")
        ret << indent << "}\n\n"
    else
        ret << call_method(clazz, method, ctor, totalparams, indent) << "\n"
    end
    # push return value
    if ctor
        ret << indent << "// return pointer to new object\n"
        ret << indent << "sq_setinstanceup(vm, 1, (SQUserPointer*)obj);\n"
        ret << indent << "//sq_setreleasehook(vm, 1, release_hook);\n"
        ret << indent << "return 1;"
    elsif method['returns']
        ret << indent << "// push return value\n"
        ret << indent << push_return(method['returns']) << "\n"
        ret << indent << "return 1;"
    else
        ret << indent << "// void method, returns no value\n"
        ret << indent << "return 0;"
    end
    return ret
  end

  def template()
  %{/*<%=gpl_license%>*/

#include "bind<%=@package['name'].downcase%>.h"
#include "bindtypes.h"
#include "bindings.h"
<% @includes.each do |include,arg| %>
#include "<%=include%>.h"<% end %>
<% @bindincludes.each do |include,arg| %>
#include "<%=include%>.h"<% end %>
namespace binding {

// object references to types in this package<% with @package['classes'] do |clazz| %>
HSQOBJECT <%=@package['name']%><%=clazz['name']%>Object;<% end %><% with @package['methods'] do |method| %>
//
// <%=@package['name']%> <%=method['name']%>
//
SQInteger <%=@package['name']%><%=method['name']%>(HSQUIRRELVM vm)
{
<%= bind_methodname nil, method['name']%>
}<% end %>
<% with @package['classes'] do |clazz| %>
//
// <%=@package['name']%>.<%=clazz['name']%> class
//<% if clazz['ctor']%>
SQInteger <%=@package['name']%><%=clazz['name']%>Ctor(HSQUIRRELVM vm)
{
<%= bind_methodname clazz, "ctor" %>
}<% end %>
<% with methodnames clazz do |methodname| %>
//
// <%=@package['name']%>.<%=clazz['name']%> <%=methodname%>
//
SQInteger <%=@package['name']%><%=clazz['name']%><%=methodname%>(HSQUIRRELVM vm)
{
<%= bind_methodname clazz, methodname %>
}
<%
end end
%>

void bind<%=@package['name']%>(HSQUIRRELVM vm)
{
    // create package table
    sq_pushstring(vm, "<%=@package['name']%>", -1);
    sq_newtable(vm);
<% with @package['methods'] do |method| %>
    // static method <%=method['name']%>
    sq_pushstring(vm, _SC("<%=method['name']%>"), -1);
    sq_newclosure(vm, &<%=@package['name']%><%=method['name']%>, 0);
    sq_newslot(vm, -3, false);
<% end %><% with @package['classes'] do |clazz| %>
    // create class <%=@package['name']%>.<%=clazz['name']%>
    sq_pushstring(vm, "<%=clazz['name']%>", -1);
    sq_newclass(vm, false);
    sq_getstackobj(vm, -1, &<%=@package['name']%><%=clazz['name']%>Object);
    sq_settypetag(vm, -1, &<%=@package['name']%><%=clazz['name']%>Object);

<% with clazz['ctor'] do |ctor| %>    // ctor for class <%=clazz['name']%>
    sq_pushstring(vm, _SC("constructor"), -1);
    sq_newclosure(vm, &<%=@package['name']%><%=clazz['name']%>Ctor, 0);
    sq_newslot(vm, -3, false);
<% end %>
    // methods for class <%=clazz['name']%><% with methodnames clazz do |methodname| %>
    sq_pushstring(vm, _SC("<%=methodname%>"), -1);
    sq_newclosure(vm, &<%=@package['name']%><%=clazz['name']%><%=methodname%>, 0);
    sq_newslot(vm, -3, false);
<% end %>
    // push <%=clazz['name']%> to <%=@package['name']%> package table
    sq_newslot(vm, -3, false);
<% end %>
    // push package "<%=@package['name']%>" to root table
    sq_newslot(vm, -3, false);
}
}}
  end
end

models = load_models

superclasses = find_superclasses(models)

models.each do|name,data|
  pack = PackageCpp.new(data, superclasses)
  filePath = "../../bindings/bind" + data["name"].downcase + ".cpp";
  write_if_changed(filePath, pack.render())
end
