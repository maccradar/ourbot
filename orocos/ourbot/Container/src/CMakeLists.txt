
#
# Components, types and plugins.
#
# The CMake 'target' names are identical to the first argument of the
# macros below, except for orocos_typegen_headers, where the target is fully
# controlled by generated code of 'typegen'.
#

# Creates a component library libContainer-<target>.so
# and installs in the directory lib/orocos/Container/
#
orocos_component(Container Container-component.hpp Container-component.cpp) # ...you may add multiple source files
#
# You may add multiple orocos_component statements.

#
# Additional headers:
#
# Installs in the include/orocos/Container/ directory
#
orocos_install_headers(Container-component.hpp) # ...you may add multiple header files
#
# You may add multiple orocos_install_headers statements.

#
# Building a Plugin
#
# Creates a plugin library libContainer-plugin-<target>.so
# and installs in the directory lib/orocos/Container/plugins/
#
# Be aware that a plugin may only have the loadRTTPlugin() function once defined in a .cpp file.
# This function is defined by the plugin and service CPP macros.
#
orocos_plugin(Container-plugin Container-plugin.cpp) # ...only one plugin function per library !
#
# You may add multiple orocos_plugin statements.


#
# Building a Service:
#
# Creates a plugin library libContainer-service-<target>.so
# and installs in the directory lib/orocos/Container/plugins/
#
orocos_service(Container-service Container-service.cpp) # ...only one service per library !
#
# You may add multiple orocos_service statements.


#
# Building a normal library (optional):
#
# Creates a library libContainer-support-<target>.so and installs it in
# lib/
#
orocos_library(Container-support support.cpp) # ...you may add multiple source files
#
# You may add multiple orocos_library statements.


