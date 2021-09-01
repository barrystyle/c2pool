git submodule update --init
g++ -O2 -std=c++2a           \
    -I./                     \
    -I./lib                  \
    -I./lib/leveldb/include  \
    -I./lib/univalue/include \
    -I./devcore              \
    -I./libnet               \
    -I./networks             \
    -I./sharechains          \
    -I./util                 \
    -I./btclibs              \
    -I./btclibs/crypto       \
    -I./coind                \
    -I./coind/p2p            \
    -I./networks             \
    -I/usr/include/python3.9 \
    -lpthread                \
    -lboost_log              \
    -lboost_filesystem       \
    -lboost_program_options  \
    btclibs/*.cpp            \
    btclibs/crypto/*.cpp     \
    c2pool/c2pool.cpp        \
    coind/*.cpp              \
    coind/jsonrpc/*.cpp      \
    coind/p2p/*.cpp          \
    dbshell/*.cpp            \
    devcore/*.cpp            \
    libnet/*.cpp             \
    networks/*.cpp           \
    sharechains/*.cpp        \
    util/*.cpp               \
    -o run_c2pool


