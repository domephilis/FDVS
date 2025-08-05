
<a id="readme-top"></a>

<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
      <ul>
        <li><a href="#built-with">Built With</a></li>
      </ul>
    </li>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#prerequisites">Prerequisites</a></li>
        <li><a href="#installation">Installation</a></li>
      </ul>
    </li>
    <li><a href="#usage">Usage</a></li>
    <li><a href="#roadmap">Roadmap</a></li>
    <li><a href="#contributing">Contributing</a></li>
    <li><a href="#license">License</a></li>
    <li><a href="#contact">Contact</a></li>
    <li><a href="#acknowledgments">Acknowledgments</a></li>
  </ol>
</details>



<!-- ABOUT THE PROJECT -->
## About The Project

[![Product Name Screen Shot][product-screenshot]]

In my study and use of financial derivatives, I have found it, at times, difficult to visualize the various variables at play (especially when trading certain strategies whose dynamics depend on either time evolution or volatility or the various other things).  Initially, I graphed things out when needed on matplotlib.  I have found that it is incredibly slow and unresponsive.  There are two parts from which speed increases may derive: 1) Faster rendering of pixels on screen; 2) Faster computation of the pricing model.  So far, I have been successful at implementing 1.  The use of an immediate mode rendered GUI and hardware-accelerated 3D rendering (through OpenGL) has provided phenomenal speed improvements.  The 3D surface translates and rotates instantaneously with mouse and keyboard inputs.  Although data generation is slow at this moment, it shouldn't be difficult to implement a significant level of parallelization through NVIDIA CUDA (https://developer.nvidia.com/gpugems/gpugems2/part-vi-simulation-and-numerical-algorithms/chapter-45-options-pricing-gpu).  NVIDIA claims that with the correct setup and hardware, one can achieve a several times speed improvement in computing the black scholes formula.  

The possibilities for this system are numerous: one can use this to visualize and track the performance of various pricing models, the dynamics of the implied volatility surface in real-time with real markets, the time evolution of the implied distributions in the markets; one can keep track of options strategies with sophisticated designs that may otherwise be difficult to understand on a 2D graph (calendar spreads) or have significant dependencies on various variables (e.g., risk free rate, market volatility, etc.); one can use this system to visualize various econometric data concurrently with monitoring one's options strategy; one can even use this system to test the performance of trading algorithms.  What this system hopes to accomplish is to allow your computer to display the hypotheses in your mind in all 3 beautiful dimensions with as few as a couple of clicks.  Development is ongoing.  I hope that this can eventually look as stunning as those 3Blue1Brown graphs.

<p align="right">(<a href="#readme-top">back to top</a>)</p>



### Built With

This section should list any major frameworks/libraries used to bootstrap your project. Leave any add-ons/plugins for the acknowledgements section. Here are a few examples.

* ImGUI
* OpenGL 3 (GLAD)
* GLFW3 (for inputs)
* CGAL (For graphics algorithms)

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- GETTING STARTED -->
## Getting Started

### Prerequisites

We require CGAL, OpenGL 3, and glfw3.

### Installation

You may compile the program yourself following the instructions below.  I have also included executables in the releases section for your convenience. (To be completed)

1. Clone the repo
   ```sh
   https://github.com/domephilis/FDVS.git
   ```
2. Install dependencies and modify the cmake file to include your library directory
   We require CGAL, OpenGL 3, and glfw3.
3. Create a build directory
   ```sh
   mkdir build
   cd build
   ```
4. Generate Makefile
   ```sh
   cmake -DCMAKE_BUILD_TYPE=RELEASE ..
   ```
5. Compile
   ```sh
   make -j12
   ```

<p align="right">(<a href="#readme-top">back to top</a>)</p>


<!-- ROADMAP -->
## Roadmap

- [] Fully link the Data Generation Interface to its backend libraries
- [] Expand methods of data generation
- [] Use of NVIDIA CUDA to speed up data generation
- [] Implement the Binomial model, Stochastic Volatility Model, etc.
- [] Add Axes Labels and Legends to the graphing system
- [] Make the graphing system more modular (create ways to make different kinds of plots, including 2D plots, histograms, scatter plots, etc.)
- [] Link the system to streamed exchange data
- [] Graph Options Implied Probability Distributions through time (3D)
- [] Install a simulated trading environment so that traders can use the software for live trading

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- CONTRIBUTING -->
## Contributing

Contributions are what make the open source community such an amazing place to learn, inspire, and create. Any contributions you make are **greatly appreciated**.

If you have a suggestion that would make this better, please fork the repo and create a pull request. You can also simply open an issue with the tag "enhancement".
Don't forget to give the project a star! Thanks again!

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

<!-- LICENSE -->
## License

Distributed under the Unlicense License. See `LICENSE.txt` for more information.

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- CONTACT -->
## Contact

Eric Sun - domephilis314159@proton.me

Project Link: [https://github.com/domephilis/FDVS](https://github.com/domephilis/FDVS/tree/master)

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- ACKNOWLEDGMENTS -->
## Acknowledgments

I've found the following resources to be incredibly helpful during the development of this project.  Links are included for your reference.

* [ImGui for User Interface Library](https://github.com/ocornut/imgui)
* [CGAL for Surface Construction](https://www.cgal.org/)
* [LearnOpenGl](https://learnopengl.com/)

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- MARKDOWN LINKS & IMAGES -->

[product-screenshot]: images/screenshot.png
