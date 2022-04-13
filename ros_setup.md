[Ros documentaiton](https://docs.ros.org/en/foxy/Installation/Ubuntu-Install-Debians.html)


Gazebo installation and setup with ros2 gazebo pkgs [here](http://gazebosim.org/tutorials?tut=ros2_installing&cat=connect_ros)

Also turtlebot setup [here](https://emanual.robotis.com/docs/en/platform/turtlebot3/quick-start/#pc-setup)

We will be working off mapping [this](https://emanual.robotis.com/docs/en/platform/turtlebot3/simulation/#gazebo-simulation)


Test mapping here:
https://emanual.robotis.com/docs/en/platform/turtlebot3/slam_simulation/


TO Save map:
cd tbot3_ws
. /install/setup.sh

```
ros2 launch turtlebot3_gazebo turtlebot3_world.launch.py

ros2 launch turtlebot3_cartographer cartographer.launch.py use_sim_time:=True

ros2 run turtlebot3_teleop teleop_keyboard

ros2 run nav2_map_server map_saver_cli -f ~/map

```
