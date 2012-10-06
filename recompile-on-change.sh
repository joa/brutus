#!/bin/bash

# Requires inotify-tools to work:
# sudo apt-get install inotify-tools

while true; do
  change=$(inotifywait -e close_write,moved_to,create . ./src)
  change=${change#./ * }
  #if [ "$change" =~ "(.+?)\.(\.h|\.cc)" ]; then 
     make
  if [ $? = 0 ]; then
    ./out/x64.release/brutus
    ./out/x64.release/brutus > ./out.dot
  else
    echo -e "\e[1;31m--- Error ---\e[m"
  fi
done
