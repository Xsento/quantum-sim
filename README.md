# Quantum Sim
Simulation of quantum states of the electron in a hydrogen atom. Renders a cloud of points, whose density and color represent the probability of finding the electron in a given place.

## Key Features
- Modern OpenGL 3D rendering
- Modern C++ STL usage
- Multithreaded generation and calculation of probability of points
- Orbital camera with zoom
- Multiplatform compilation and linking (Windows and Linux)
- ImGUI based setup interface


## Physics Background
The simulation visualizes the electron probability density $\lvert\psi_{n,l,m}(r,\theta,\phi)\rvert^2$ obtained by solving the time-independent Schrödinger equation for a hydrogen-like atom. 
For more about the Schrödinger equation solution for a hydrogen-like atom please refer to <a href="https://faculty.washington.edu/seattle/physics227/reading/reading-26-27.pdf"> this article </a>

Positions are sampled using a **Monte Carlo / Rejection Sampling** algorithm.

Currently supported quantum numbers:
- Principal ($n$): $1, 2, 3, 4$
- Azimuthal ($l$): $0 \le l < n$
- Magnetic ($m$): $-l \le m \le l$

## Controls
| Function | Key |
|---|---|
| **Rotate Camera** | `Arrow Keys` |
| **Zoom In/Out** | `Z/X` |
| **Exit** | `Escape` |

## Screenshots
All images come from 100m starting points (~450k - ~900k points visible)
|  State  | Visualization |
|---|---|
| <nobr>**$n=2, l=1 ,m=0$**</nobr> | <img width="1205" height="1007" alt="210" src="https://github.com/user-attachments/assets/f22065ea-465a-4fd5-bb76-8022ff49ce68" /> |
| <nobr>**$n=3, l=2, m=0$**</nobr> | <img width="1205" height="1007" alt="320" src="https://github.com/user-attachments/assets/9992f519-d357-4d56-bf56-7f96342a6db5" /> |
| <nobr>**$n=3, l=2, m=1$**</nobr> | <img width="1205" height="1007" alt="321" src="https://github.com/user-attachments/assets/0d4b0bff-b344-42ae-bc69-04969699c1ac" /> |
| <nobr>**$n=4, l=2, m=0$**</nobr> | <img width="1205" height="1007" alt="420" src="https://github.com/user-attachments/assets/8e6075fc-00e2-47e5-82a3-ca2f54b875c3" /> |

## Dependencies
- CMake `3.28.3+`
- C++ `20`
- OpenGL `3.3+`
- GLM  `included in the repository`
- GLFW3 `pre installed for Windows`
- GLAD `included in the repository`
- ImGUI `included in the repository`

## GLFW3 Installation
Debian/Ubuntu
```
sudo apt update
sudo apt install libglfw3 libglfw3-dev
```

## Compilation and Execution
Linux/Windows
```
mkdir build
cmake -S . -B build 
cmake --build build
cd build
```
Linux
`./quantum-sim` <br>
Windows
`.\quantum-sim.exe`

## Performance & Optimizations
The engine has been heavily optimized to handle massive point clouds smoothly by eliminating common CPU and GPU bottlenecks:

- **Data-Parallel Processing:** Utilizes `std::thread` to split the random point generation and probability evaluation across all available CPU cores, scales dynamically with hardware thread count.
- **Lock-Free Pipeline:** Generation threads write directly into independent sub-vectors, eliminating thread contention.
- **Batch Rendering (VBO):** Transitioned from heavy individual draw calls to a single continuous interleaved **Vertex Buffer Object (VBO)** storing both position `vec3` and probability `float` attributes.
- **Results:** Reduced CPU calculations time by **~86%** (100m points generated and calculated in ~30 seconds on a low end machine) and shifted the rendering bottleneck entirely to the GPU, increasing frame rates from **8-12 FPS to a stable 60+ FPS (V-Sync capped)** on integrated laptop graphics.
