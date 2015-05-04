banner $(date)
g++ -Wall -Wextra -std=c++14 -Ilib -o umdecode src/umdecode.cc src/CCCHDecoder.cc src/Burst.cc src/ChannelInfo.cc lib/airprobe/cch.c lib/airprobe/fire_crc.c
