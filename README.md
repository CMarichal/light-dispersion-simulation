## Simulating light dispersion on a prism

# Background
Simple one-path raytracing performs light simulation using a geometrical approach. In this approach, light is composed of only one color that can change when performing diffusion, reflection, or refraction. Thus, it does not consider the physical wave nature of the light and cannot simulate the light dispersion phenomenon encountered with prisms.

# Problem
This project aims to implement a dispersive model using one-pass raytracing to compute and display light dispersion on a purely specular prism. It will be using properties from real glass used in optics and adapt the model as required.

# Technical structure

The project is implemented using C++11 and OpenGLMathematics. It is possible to use the project with different graphics librabry by implementing the provided interfaces. The one used here is SFML.

# References

[1] S. Cropp and E. Zhang. Light Refraction with Dispersion. 2014. URL: https://www.cs.rpi.edu/~cutler/classes/advancedgraphics/S14/final_projects/eric_steven.pdf.

[2] F Kenton Musgrave. Prisms and Rainbows: a Dispersion Model for Computer Graph-ics. In Graphics Interface ', pages 227{234, Ontario, Canada, 1989.

[3] Yinlong Sun, F. David Fracchia, and Mark S. Drew. Rendering Light Dispersion With A Composite Spectral Model. Diamond, 2(37.17):1{44, 2000.

[4] Spencer W. Thomas. Dispersive refraction in ray tracing. The Visual Computer, 2(1):3{8, January 1986. doi:10.1007/BF01890982.

[5] Andrea Weidlich and Alexander Wilkie. Anomalous Dispersion in Predictive Rendering. Computer Graphics Forum, 28(4):1065{1072, 2009. doi:10.1111/j.1467-8659.2009.01483.x.
