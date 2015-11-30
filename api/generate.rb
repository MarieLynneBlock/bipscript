#!/usr/bin/ruby

require 'sqapigen'

sqapi = Sqapigen.new

model_dir = "."
output_dir = "../bindings"
sqapi.generate_bindings model_dir, output_dir
