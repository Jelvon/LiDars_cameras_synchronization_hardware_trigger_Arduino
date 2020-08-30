# Lidars_cameras_synchronization_hardware_trigger
How it works:
![image](https://github.com/Jelvon/Lidars_cameras_synchronization_hardware_trigger/blob/master/image/trigger_structure.png)

## Hardware version
* Lidar version:https://www.hesaitech.com/en/Pandar40P

* Camera version:https://en.ids-imaging.com/store/ui-3160cp-rev-2-1.html

* Trigger Cable:https://en.ids-imaging.com/store/i-o-standard-cable-straight-5-m.html

* Arduino yun:https://store.arduino.cc/arduino-yun-rev-2

* protection circuit

## Sensor Network
![image](https://github.com/Jelvon/Lidars_cameras_synchronization_hardware_trigger/blob/master/image/cidclip_image001.png)
* Car PC: PTP master
* LiDars: PTP slave
* Arduino Yun: PTP slave

Every time the Arudino is synchronized with the PTP network, it would generate a pps signal to its MCU and 20/random trigger signals would be generated to trigger the cameras and meanwhile the Lidars have already been triggered by the PTP synchronization signal. We also designed a ros publish functionality for the Arduino Yun that everytime the pps signal is generated and the arudino linux part is synchronized, a ros message with timestamp information would be published. This timestamp information can be used for checking the pps generating delay and according to that we are able to make some delay compensation in the later development if necessary.

## Hardware Prerequisite:
* Lidar is PTP enabled. (Normally hardware PTP enabled)
* Car PC has NIC (Network interface controller) which is PTP hardware/software enabled. (Normally hardware PTP enabled)
* Trigger board is PTP enabled. (Normally software PTP enabled, hardware enabled is expensive and not available in Arudino or the arduino Peripherals）
* If camera is GigE version （support IEEE 1588/PTP） which is PTP enabled, we don’t have to use the Arduino here.

About GNSS/GPS:
* GPS could provide accurate time resource here, but in our application case it is not essential, we only need relative same time between sensors and PC.
* GPS could be used to trigger LiDar and Arduino also.
## Lidar configuration
Turn the ptp trigger mode on in the web page control panel
![image](https://github.com/Jelvon/Lidars_cameras_synchronization_hardware_trigger/blob/master/image/0.png)
## PC configuration
1. install linuxptp

```
sudo apt-get install linuxptp net-tools ethtool 
```

2. check portal name of NIC

```
ifconfig
```
![image](https://github.com/Jelvon/Lidars_cameras_synchronization_hardware_trigger/blob/master/image/1.png)

3. check if the NIC would support hardware/software ptp

```
Ethtool -T enp0s3`
```
![image](https://github.com/Jelvon/Lidars_cameras_synchronization_hardware_trigger/blob/master/image/2.png)

4. activate ptp master(car pc) here

```
sudo ptp4l -A -2 -S -m -i enp0s3`
```
![image](https://github.com/Jelvon/Lidars_cameras_synchronization_hardware_trigger/blob/master/image/3.png)

so car pc ptp configuration has been finished.

## Arduino Yun Configuration
1. connect the Yun with power supply and configure it according to steps in the following link, you will get the wifi well-configured afterwared so that you could access to the Yun through SSH:

```
https://www.arduino.cc/en/Guide/ArduinoYun#toc16 
```

2. document the Yun's IP (ifconfig),

hint:install arduino IDE in your pc and connect the UART between Yun's MCU and Linux core according to the following link:

```
https://www.twilio.com/blog/2015/02/arduino-wifi-getting-started-arduino-yun.html
```
3. We have to expand the storage space due to the limitation space of Yun itself.

expand the yun linux storge with a sd card (>4G) and run the expand_yun.ino (https://github.com/Jelvon/Lidars_cameras_synchronization_hardware_trigger/blob/master/no_ros_message_version_file/expand_yun.ino) in the arduino IDE

hint:the details to configure this expansion according to the following link:
```
https://www.arduino.cc/en/Tutorial/ExpandingYunDiskSpace

```
4. access to arduino using ssh and configure the Yun's linux system now, conduct the following commands:

hint:this will take normally > 30 mintues, be careful of the log report to make sure that there are no mistakes.

```
opkg update
```
```
opkg install linuxptp libgcc openssh-sftp-server binutils make gcc
```
5. if the car pc is in PTP master mode, run the following commmands in Yun:

hint:when you see the screen output is like the follwing picture, the ptp slaves of Yun linux and car pc have been well-configured

```
ptp4l -A -2 -S -m -i eth1

```
![image](https://github.com/Jelvon/Lidars_cameras_synchronization_hardware_trigger/blob/master/image/4.png)

## Yun Linux generate PPS for MCU
1. Yun does not support generating PPS using kernel,so we can only generate PPS in the userspace.

This will generate uncertainty of time of generating, my solution is that: we publish the time of signal generating through ROS frame. In this case, we would know the exact time to generate PPS for all the ROS nodes.

in the Yun linux, we conduct the following commands:

```
echo 1 > /sys/class/gpio/gpio21/value
```
```
echo 26 > /sys/class/gpio/export
```
```
echo "high" > /sys/class/gpio/gpio26/direction
```
```
echo 0 > /sys/class/gpio/gpio26/value
```
```
mkdir /root/a_time
```
```
cd /root/a_time
```
2. use scp copy the a_time.c(https://github.com/Jelvon/Lidars_cameras_synchronization_hardware_trigger/blob/master/no_ros_message_version_file/a_time.c) into the /root/a_time, continue to conduct the following command to compile the c file:
```
gcc a_time.c -o get_atime
```
```
./get_atime
```
3. if you can see the LED flash according to the parameter set-up in the a_time.c file, it means the PPS generation is successful

Every time you reboot the arduino, this operation would be erased and you have to do it again, one solution is that:

in the file /etc/rc.local,before the exit0, add:

```
ptp4l -A -2 -S -m -i eth1
```
```
echo 1 > /sys/class/gpio/gpio21/value
```
```
echo 26 > /sys/class/gpio/export
```
```
echo "high" > /sys/class/gpio/gpio26/direction 
```
```
echo 0 > /sys/class/gpio/gpio26/value
```
```
/root/a_time/get_atime
```

## Trigger MCU Configuration
Now,we have to configure the triggers of the cameras
upload the camera_trig.ino to(https://github.com/Jelvon/Lidars_cameras_synchronization_hardware_trigger/blob/master/no_ros_message_version_file/camera_trig.ino) Yun MCU through Arduino IDE
Now the program is in the N-slaves mode
change the paramters inside to change the trigger mode
```c
#define TRIG_GAP 200        //gap between trigger signals
#define TRIG_WIDTH 10       //trigger signal width
#define MAX_TRIG_PER_SEC 5  //the trigger times every one PPS signal

```
## ROS message of timestamp generation and optimized version of the configuration solution
there are 3 solutions to generate ros message in the ROS frame

1.install ROS in Yun

2.fake a ROS/UDP packet

3.use a thrid-lib to fake a ROS node withour ROS frame

I tried 3 solutions, but only NO.3 is finished, NO,1 failed due to the weakness of Yun, ROS is too large for it, NO.2 is not universal, every small adjustment afterwards will bring bad performance of the configuration.

the newly published roslibpy support so.
1. in our car pc (ros master),we should do:

```
sudo apt-get install -y ros-kinetic-rosbridge-server
```
```
sudo apt-get install -y ros-kinetic-tf2-web-republisher
```
2. run the rosbridge server
```
roslaunch rosbridge_server rosbridge_websocket.launch 
```
```
rosrun tf2_web_republisher tf2_web_republisher

```
3. in our Yun linux, copy the trigger-gen folder(https://github.com/Jelvon/Lidars_cameras_synchronization_hardware_trigger/tree/master/ros_message_version_file/trigger-gen) after uncompression into /root in Yun using scp

4. ssh access to Yun linux and conduct:
```
cd /root/trigger-gen
```
```
opkg install python-pip
```
```
opkg install python-lib2to3_2.7.14-5_mips_24kc.ipk
```
```
opkg --force-overwrite install python-dev_2.7.14-5_mips_24kc.ipk
```
```
opkg install python-ply_3.9-1_mips_24kc.ipk
```
```
opkg install python-pycparser_2.14-3_mips_24kc.ipk opkg install python-cffi_1.8.3-1_mips_24kc.ipk
```
```
opkg install python-idna_2.1-1_mips_24kc.ipk
```
```
opkg install python-enum34_1.1.6-2_mips_24kc.ipk opkg install python-ipaddress_1.0.17-1_mips_24kc.ipk opkg
```
```
install python-pyasn1_0.1.9-1_mips_24kc.ipk opkg install python-six_1.10.0-1_mips_24kc.ipk
```
```
opkg install libopenssl1.1_1.1.1f-1_mips_24kc.ipk
```
```
opkg install python-cryptography_2.8-1_mips_24kc.ipk
```
```
TMPDIR=/mnt/sda1/pip_tmp/ pip install --cache-dir=/mnt/sda1/pip_tmp/ --build /mnt/sda1/pip_tmp/ roslibpy
```
```
gcc –o get_time get_time.c
```
* Now we have an optimized version of this workflow and I would recommend you to use it if you could finish and understand the above steps.

* the old c file shall be dropped, we use the new c file in the folder(https://github.com/Jelvon/Lidars_cameras_synchronization_hardware_trigger/tree/master/ros_message_version_file) above

* we use python file to send the ros message and also invoke the c file

* the python file also solves the GPIO initialization after rebooting and open the ptp

* the c file is used for PPS generation and also camera trigger signal generation

* in the trigger-gen.py file(https://github.com/Jelvon/Lidars_cameras_synchronization_hardware_trigger/tree/master/ros_message_version_file/trigger-gen/trigger-gen.py) we should configure the ip adress of the host which runs the rosbridge server

```
client = roslibpy.Ros(host='192.168.0.48', port=9090)
```
Hint: this is the case in the arduino if you want to run the python file, conduct:
```
python trigger-gen.py
```
then then you will see this and it proves that you are succussful.
![image](https://github.com/Jelvon/Lidars_cameras_synchronization_hardware_trigger/blob/master/image/5.png)

more details please refer to:
```
https://roslibpy.readthedocs.io/en/latest/index.html
```
## When you have memory crash problem in the opkg install step...
![image](https://github.com/Jelvon/Lidars_cameras_synchronization_hardware_trigger/blob/master/image/6.png)
try this:
```
dd if=/dev/zero of=/swapfile bs=1024 count=524288
```
```
chown root:root /swapfile
```
```
chmod 0600 /swapfile
```
```
mkswap /swapfile
```
```
swapon /swapfile
```
## cabling between cameras and arduino yun
![image](https://github.com/Jelvon/Lidars_cameras_synchronization_hardware_trigger/blob/master/image/cidclip_image007.png)

