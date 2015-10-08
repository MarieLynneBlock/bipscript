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

def with value
    if value.kind_of?(Array) then
        counter = 0
        value.each do |x|
            yield(x, counter += 1)
        end
    elsif value
        yield(value, 1)
    end
end

def load_models
    models = Hash.new
    Dir.glob('../*.yaml').sort.each do|f|
        models[f] = YAML.load_file(f)
    end
    return models
end

def find_superclasses models
    superclasses = Hash.new
    models.each do|name,data|
       with data['classes'] do |clazz|
          clazz['package'] = data['name']
          with clazz['super'] do |superclass|
            list = superclasses[superclass]
            if not list
                list = Array.new
                superclasses[superclass] = list
            end
            list << clazz
         end
       end
    end
    return superclasses
end

def find_includes superclasses
    includes = Hash.new
    superclasses.each do |superclass,subclasses|
        subclasses.each do |subclass|
            with subclass['include'] do |include|
               includes[include] = true
            end
        end
    end
    return includes
end


def cppname clazz
    clazz['cppname']?clazz['cppname']:clazz['name']
end

def gpl_license
 "\n * This file is part of Bipscript.
 *
 * Bipscript is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Bipscript is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Bipscript.  If not, see <http://www.gnu.org/licenses/>.
 "
end

def write_if_changed filename, contents
    if !File.exists?(filename) or contents != File.read(filename)
        puts "writing changed file " + filename
        File.write(filename, contents)
    end
end
