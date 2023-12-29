
set(SRCROOT ${PROJECT_SOURCE_DIR}/examples/ClientTCP)

# all source files
set(SRC ${SRCROOT}/Client.cpp)

# define the sockets target
sfml_add_example(Client
                 SOURCES ${SRC}
                 DEPENDS sfml-network -lpthread)
