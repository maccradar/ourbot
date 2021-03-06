
#
# Components, types and plugins.
#
# The CMake 'target' names are identical to the first argument of the
# macros below, except for orocos_typegen_headers, where the target is fully
# controlled by generated code of 'typegen'.
#

# Creates a component library libReference-<target>.so
# and installs in the directory lib/orocos/Reference/
#
orocos_component(Reference Reference-component.hpp Reference-component.cpp)
#
# You may add multiple orocos_component statements.

#
# Additional headers:
#
# Installs in the include/orocos/Reference/ directory
#
orocos_install_headers(Reference-component.hpp)
#
# You may add multiple orocos_install_headers statements.

#
# Building a Plugin
#
# Creates a plugin library libReference-plugin-<target>.so
# and installs in the directory lib/orocos/Reference/plugins/
#
# Be aware that a plugin may only have the loadRTTPlugin() function once defined in a .cpp file.
# This function is defined by the plugin and service CPP macros.
#
orocos_plugin(Reference-plugin Reference-plugin.cpp) # ...only one plugin function per library !
#
# You may add multiple orocos_plugin statements.


#
# Building a Service:
#
# Creates a plugin library libReference-service-<target>.so
# and installs in the directory lib/orocos/Reference/plugins/
#
#orocos_service(Reference-service Reference-service.cpp) # ...only one service per library !
#
# You may add multiple orocos_service statements.


#
# Building a normal library (optional):
#
# Creates a library libReference-support-<target>.so and installs it in
# lib/
#
orocos_library(Reference-support support.cpp) # ...you may add multiple source files
#
# You may add multiple orocos_library statements.


