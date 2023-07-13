# SOCRATES

<img align="right" width="300" height="300" src="https://github.itap.purdue.edu/RadioNavigationLab/SOCRATES/blob/master/code/source/matlab/icon/SOCRATES-logo.png">
SOCRATES (Signals of Opportunity Constellation and Remote sensing Analysis Tool for Earth Science) is an end-to-end mission simulator for multiple Signals of Opportunity (multi-SoOp) for land remote sensing. This tool offers a comprehensive analysis environment to explore from a high-level mission design to scientific measurements. The current release of SOCRATES boasts the following capabilities:

  - Satellite- and tower-based observation of soil moisture using multi-SoOp.
  - Tradespace exploration of multi-SoOp constellation design.
  - Orbit propagation and import of multiple receivers and transmitters.
  - Specular reflection point prediction on the Earth surface.
  - Instrument property realizations including channel frequency, bandwidth, coherent integration time, and noise temperature.
  - Antenna property realizations including antenna orientation and gain pattern.
  - Geophysical data realizations including soil moisture, soil temperature, soil texture, vegetation, land cover, freeze/thaw state, and snow cover.
  - Link budget for direct and reflected signals.
  - Modeling of multi-body spacecraft attitude dynamics with rigid bodies.
  
Use SOCRATES-Retrieval for end-to-end retrieval analysis using multi-SoOp. SOCRATES-Retrieval can be accessed from the following Code Ocean capsule: <https://doi.org/10.24433/CO.5405959.v1>

## Compilation

The GNU Compiler Collection with C11 standard on a Unix or Linux platform is recommended to compile the code. Additional software packages OpenMPI and netCDF are required for the compilation. The latest versions of the packages are recommended. OpenMPI is used for multi-processing the mission design processes. NetCDF is for creating, reading, and writing the NetCDF files.

SOCRATES can be accessed from the following GitHub repository: <https://github.itap.purdue.edu/RadioNavigationLab/SOCRATES>. To install from GitHub:

  1. Download the source code from the latest release or clone the code from GitHub: `$ git clone git@github.itap.purdue.edu:RadioNavigationLab/SOCRATES.git`
  2. Open a terminal on the top-level directory (i.e., .../SOCRATES) and run `$ make` to compile the simulator. Note that `$ make clean` deletes the existing object files and the executable file.
  3. An executable file SOCRATES should be created in the top-level directory.

## Downloading Large Binaries
In the current version, the following large files must exist in each corresponding directory:

  - A 1-km resolution land mask: ./earthdata/static/Land_Mask_1km_EASE2.uint8

These files are distributed through releases. To download:

  1. Visit <https://github.itap.purdue.edu/RadioNavigationLab/SOCRATES/releases> and click the latest release.
  2. Download the necessary large files attached to the release.
  3. Place each file in each corresponding directory.

## Getting Started
See the manual, "SOCRATES\_User\_Manual.pdf" in the ./doc folder.

## Citation
S. Kim and J. L. Garrison, "A Mission Design Tool for Satellite Constellations Using Multi-Frequency Signals of Opportunity," IGARSS 2023 - 2023 IEEE International Geoscience and Remote Sensing Symposium, 2023.
