/*********************************************************************************************************************
Copyright (c) 2020 RoboSense
All rights reserved

By downloading, copying, installing or using the software you agree to this license. If you do not agree to this
license, do not download, install, copy or use the software.

License Agreement
For RoboSense LiDAR SDK Library
(3-clause BSD License)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following
disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following
disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the names of the RoboSense, nor Suteng Innovation Technology, nor the names of other contributors may be used
to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*********************************************************************************************************************/


#include "manager/node_manager.hpp"

#include <rs_driver/macro/version.hpp>
#include <signal.h>



#include <rclcpp/rclcpp.hpp>


using namespace robosense::lidar;

std::mutex g_mtx;
std::condition_variable g_cv;


using namespace robosense::lidar;

static void sigHandler(int sig)
{
  RS_MSG << "RoboSense-LiDAR-Driver is stopping....." << RS_REND;
  rclcpp::shutdown();
}

int main(int argc, char** argv)
{
 
  signal(SIGINT, sigHandler);  ///< bind ctrl+c signal with the sigHandler function

  RS_TITLE << "********************************************************" << RS_REND;
  RS_TITLE << "**********                                    **********" << RS_REND;
  RS_TITLE << "**********    RSLidar_SDK Version: v" << RSLIDAR_VERSION_MAJOR 
    << "." << RSLIDAR_VERSION_MINOR 
    << "." << RSLIDAR_VERSION_PATCH << "     **********" << RS_REND;
  RS_TITLE << "**********                                    **********" << RS_REND;
  RS_TITLE << "********************************************************" << RS_REND;


  rclcpp::init(argc, argv);

  auto node = std::make_shared<rclcpp::Node>("rslidar_sdk_node");

  std::string config_path;
  if (argc > 1) {
    for (int i = 1; i < argc; ++i) {
      if (std::string(argv[i]) == "-c" && i + 1 < argc) {
        config_path = argv[i + 1];
        break;
      }
    }
  }

  if (config_path.empty()) {
    RS_ERROR << "Config path parameter not found. Please provide a valid config_path parameter." << RS_REND;
    return -1;
  }

  YAML::Node config;
  try
  {
    config = YAML::LoadFile(config_path);
  }
  catch (...)
  {
    RS_ERROR << "The format of config file " << config_path 
      << " is wrong. Please check (e.g. indentation)." << RS_REND;
    return -1;
  }

  std::shared_ptr<NodeManager> demo_ptr = std::make_shared<NodeManager>();
  demo_ptr->init(config);
  demo_ptr->start();

  RS_MSG << "RoboSense-LiDAR-Driver is running....." << RS_REND;

  std::unique_lock<std::mutex> lck(g_mtx);
  g_cv.wait(lck);


  return 0;
}
