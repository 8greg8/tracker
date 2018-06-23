# Flow features

Application for testing and using different trackers.


## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. 


### Prerequisites

Before installing this software you need:

* [CMake](https://cmake.org) - Tool for building the project v3.11

* [OpenCV](http://opencv.org/) - OpenCV library v3.1
  
  Links:
    * [github - opencv](https://github.com/opencv/opencv) 


* [Boost](http://www.boost.org/) - Boost library >v1.53.0
* [Dlib](http://www.dlib.net/) - Dlib C++ library v19.2

<aside class="notice">
**Libccv** and **LibOpenTLD** are also required but are already included in the project. You don't have to install them.
</aside>

For smoother installing and usage I recommend you to use [Anaconda environment](https://anaconda.org). When you install it you can use [conda_config.sh](conda_config.sh) to configure new `tracker` environment

### Installing

1. Download or clone this project
2. Change `installDir` in [`build.sh`](build.sh)
3. Run 'build.sh'


### Running programs

Run `cv_tracker` or `custom_tracker`


### System

Tested only on CentOS Linux 7.3


## Contributing

For contributing contact authors.


## Versioning

For the versions available, see the [tags on this repository](https://github.com/8greg8/tracker/tags). 


## Authors

* **Janez Perš** - *Basic idea behind this project and team leader* - [jpers1](https://github.com/jpers1)
* **Gregor Koporec** - *Developed and tested main part of software* - [8greg8](https://github.com/8greg8)


## License

This project is licensed under the GNU GPL License - see the [LICENSE](LICENSE) file for details.


## Acknowledgments

Forked projects [ccv](https://github.com/8greg8/ccv) and [OpenTLD](https://github.com/8greg8/OpenTLD) are used. Credits go to authors of original projects. Modified code is documented in commits of forks. Source code was not changed. Changes were made in files for compiling the projects to better integrate them to our compiling process.

