# Script that handles serialSend.cpp, configuring Linux's UART.
# Please fulfill the following variables with the values you consider
# to be correct for your serial communication. 

device=/dev/ttyACM0
baudrate=115200
databits=8
parity=none
stop=1
softwareHandshake=0 # 0 - softwareHandshake=false / 1 - softwareHandshake=true
hardwareHandshake=0 # 0 - hardwareHandshake=false / 1 - softwareHandshake=true
openRead=1    # 0 - open file for reading = false / 1 - open file for reading = true
openWrite=1   # 0 - open file for writing = false / 1 - open file for writing = true
applyNow=1    # 0 - apply settings above now = false / 1 - apply settings above now = true
filename=$1   # Takes file to be send as an argument
charDelay=500 # Delay between sent bytes in microseconds
logFile=/home/javigon/work/XBeeWSC #Absolute route

# If the route for the file to be send ($filename) is relative, we transform it to absolute
mypath=$PWD

cd $(dirname $filename)
filename=$PWD/$(basename $filename)
cd $mypath

# We call the c++ serialSend
$mypath/serialSend $device $baudrate $databits $parity $stop $softwareHandshake $hardwareHandshake $openRead $openWrite $applyNow $filename $charDelay $logFile