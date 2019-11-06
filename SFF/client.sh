#!/bin/bash
for i in 1 2 3 4
do 
	osascript -e 'tell application "Terminal" to do script "cd /Users/prakash/Desktop/Concurrent-Webserver/SFF; ./wclient localhost 10000 1 /spin.cgi"'
done
