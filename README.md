# Lidars_cameras_synchronization_hardware_trigger
How it works:

Lidar version:https://www.hesaitech.com/en/Pandar40P
Camera version:https://en.ids-imaging.com/store/ui-3160cp-rev-2-1.html

Car PC: PTP master
LiDars: PTP slave
Arduino Yun: PTP slave
Every time the Arudino is synchronized with the PTP network, it would generate a pps signal to its MCU and 20/random trigger signals would be generated to trigger the cameras and meanwhile the Lidars have already been triggered by the PTP signal.

Hardware prerequisite:
Lidar is PTP enabled.
Car PC has NIC (Network interface controller) which is PTP hardware/software enabled.
Trigger board is PTP enabled.
If camera is GigE version which is PTP enabled, we don’t need the Arduino.

About GNSS/GPS:
GPS could provide accurate time resource here, but in our application case it is not essential, we only need relative same time between sensors and PC.
GPS could be used to trigger LiDar and Arduino also.
