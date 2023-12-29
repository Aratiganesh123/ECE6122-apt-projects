set(SRCROOT ${PROJECT_SOURCE_DIR}/examples/ServerTCP)

# all source files
set(SRC ${SRCROOT}/Server.cpp)

# define the sockets target
sfml_add_example(Server
                 SOURCES ${SRC}
                 DEPENDS sfml-network -lpthread)
