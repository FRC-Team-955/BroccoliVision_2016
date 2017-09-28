#stty -F /dev/ttyACM0 cs8 115200 ignbrk -brkint -icrnl -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke noflsh -ixon -crtscts
./setexposure.sh 50
./build/BroccoliBot_2016 ./Settings.json realsense 617203005698
