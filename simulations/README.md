# Urban CFD

This folder contains toy simulations for urban CFD applications using OpenFOAM.

1. **Single tree**: steady-state simulation with porous media model for a single tree in a empty domain.

**Final modelling goal (urban CFD setup)**

* Incompressible unsteady **RANS** with buoyancy (possibly with solution-driven Adaptive Mesh Refinement, driven by dispersion processes and/or turbulence model) .
* Multi-component mixture (dry air + water vapour).
* Heat transfer with radiation
* Pollutant transport with emissions and deposition modelling.
* Vegetation modelling via porous regions and wall functions.
* Semantic surfaces for wall functions (roughness, thermal/ radiative properties, deposition-linked properties).
