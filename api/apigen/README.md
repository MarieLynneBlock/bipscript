
# APIGen docs

## Model

### packages

YAML files hold the package models, one package per file with the following fields:

- name: name of the package as bound to the scripting environment

- methods: list of static methods in this package

- classes: list of classes in this package

### classes

each class has the following fields:

- name: name of this class as bound in the scripting environment

- cppname: name of this class in the c++ source

- include: include file(s) for this c++ source

- ctor: constructor(s) for this class

### constructors

the constructor model has the following fields:

- parameters: parameters for this constructor

### methods

methods have the following fields:

- name: method name as bound to the script environment

- parameters: list of parameters for this method

- returns: type of the return value

### parameters

- name: parameter name

- type: parameter type


