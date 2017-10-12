/* Copyright 2017 The sfcpp Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/


#include "ImageLogger.hpp"

namespace sfcpp {
namespace img
{

/*
    Constructor
*/
ImageLogger::ImageLogger(bool i_write_switch)    :
    write_switch(i_write_switch),
    image_count(0)
{
    time_t rawtime;
    struct tm * timeinfo;
    char scoop_destination_name[80];

    time (&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(scoop_destination_name, 80, "%d-%m-%Y_%H-%M-%S", timeinfo);
    ImageLogger::destination_name = scoop_destination_name;

#ifndef _WIN32
    std::string dir_name = "./data/ImageLog/"+ImageLogger::destination_name;
    mkdir("./data/ImageLog", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    mkdir(dir_name.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif
}


} /* namespace img */
}
