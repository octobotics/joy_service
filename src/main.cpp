#include <ros/ros.h>
#include <std_srvs/SetBool.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

pid_t joy_node_pid = -1;

bool handleJoyService(std_srvs::SetBool::Request &req, std_srvs::SetBool::Response &res)
{
    if (req.data)
    {
        if (joy_node_pid == -1)
        {
            
            joy_node_pid = fork();
            if (joy_node_pid == 0)
            {
                
                execlp("rosrun", "rosrun", "joy", "joy_node", (char *)NULL);
                _exit(EXIT_FAILURE); 
            }
            else if (joy_node_pid > 0)
            {
                
                res.success = true;
                res.message = "Started joy_node";
            }
            else
            {
                res.success = false;
                res.message = "Failed to fork process";
            }
        }
        else
        {
            res.success = false;
            res.message = "joy_node is already running";
        }
    }
    else
    {
        if (joy_node_pid != -1)
        {
           
            if (kill(joy_node_pid, SIGTERM) == 0)
            {
                waitpid(joy_node_pid, NULL, 0); 
                joy_node_pid = -1;
                res.success = true;
                res.message = "Stopped joy_node";
            }
            else
            {
                res.success = false;
                res.message = "Failed to stop joy_node";
            }
        }
        else
        {
            res.success = false;
            res.message = "joy_node is not running";
        }
    }
    return true;
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "joy_service_node");
    ros::NodeHandle nh;

    ros::ServiceServer service = nh.advertiseService("set_joy", handleJoyService);

    ROS_INFO("Ready to handle joy service requests.");
    ros::spin();

    return 0;
}
