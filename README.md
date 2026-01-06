
## Repository structure
- examples of grid generation from available *stl* models
- examples of dummy simulations with vegetation modelling
- OpenFOAM libraries

## Introduction
We aim to build a digital twin for an urban environment. The base step to achieve such goal is performing an evaluation of the computational model that is going to predict all physical phenomena involved. 

Model evaluation involves a step-by-step validation procedure to access the model's ability to capture key characteristics of the so called *Atmospheric Boundary Layer*. 

The validation strategy adopted is hierarchical and with that we progressively increase the "difficulty": we start from simple-shaped obstacles with no dispersion, no thermal stratification, no heat transfer, no porous media, and then we add all of these "features" ending up with simulations in real-cities.

Alongside all the comparative analysis required to access computational model's accuracy and reliability, we need to develop a tool to automatically reconstruct those urban environments we need, in the way we need.

Since is no new thing the reconstruction of city-like 3D-models for CFD simulations we start from promising projects, discussed here below.


### Starting point
- city4CFD

### Data recovery
- [Open Street Map](https://www.openstreetmap.org)
- [GeoScopio](https://www.regione.toscana.it/-/geoscopio)

#### Additional data
- [Open Data Firenze](https://opendata.comune.fi.it/content/pubblicati-i-nuovi-dataset-dtm-2023-e-dsm-2023)

### Project structure
- Report the current state of city4CFD implementation with reference to its documentation and to the papers that explain its features and use papers that show its usage.

- [Use case for automatic LOD 2 building reconstructions](https://www.tudelft.nl/en/2021/bk/all-10-million-buildings-in-the-netherlands-available-as-3d-models)

- Define possible improvements and extensions of the current implementation:
    - how do we obtain a city footprint map for LOD2 reconstruction? To achieve that level of detail seems required the use of roofer algorithm
    - exploit what is already implemented to build a city model with LOD2.
    - create a library to use different stl models for each recorded type of natural element (e.g. Trees, bushes, water surfaces, streets, etc.)
    - create a dictionary file to store library models info/parameters used to build the city model from a given footprint map (e.g. Each tree model has a time-varying parameter called "LAD" which is the Leaf Area Density)

- Once we're have a pipeline to reconstruct a city model with LOD2, we can use it to build a city model for Firenze, Italy, and then use it to run a CFD simulation

- A proof of concept simulation will be run to test the differences changing the LAD parameter as a function of time (e.g. In winter, the LAD is lower than in summer)


### References
- [City4CFD repository](https://github.com/tudelft3d/City4CFD/)
- [Building footprints from Open Street Map](https://tudelft3d.github.io/3dfier/building_footprints_from_openstreetmap.html)
- [open list of semantic 3D city model](https://github.com/OloOcki/awesome-citygml)


### Additional notes
Since your thesis focuses on vegetation modelling, it will be useful (at least in the literature review) to explore how vegetation effects are represented in CFD simulations of large-scale urban domains.

Vegetation can influence urban flows through several mechanisms (simplified here):

- Drag force (momentum sink)
- Turbulence generation (turbulence source)
- Chemical deposition (mass sink)
- Heat transfer (heat source/sink)
- Transpiration (humidity source)
- Radiation attenuation (shadowing effect)

You are not expected to implement all of these in OpenFOAM, but it’s important to acknowledge them and understand their relevance. This broader perspective will help you frame the vegetation model in the context of urban digital twins, where species-dependent parameters that vary seasonally need to be tabulated.

From a CFD point of view:

- Vegetation with height (trees, bushes, hedges): usually represented as porous regions. Their impact on momentum and turbulence equations will be captured through source terms; the same is for chemical deposition, transpiration, and heat transfer, with specific source terms in the respective transport equations for chemicals, water vapour, and temperature.

- Low vegetation (grass, crops, open fields): often represented via custom boundary conditions, which account for the same effects but applied at the surface level rather than in a volume.

This way, you’ll have a clear overview of what to consider when modelling vegetation.


## Thesis project
**Topic**
Vegetation modelling for urban micro-climate simulations at urban scale.

**Research gap**
Despite many studies on vegetation effects in urban CFD, there is no end-to-end, scalable, species-aware workflow that:

1. automatically builds 3D city/mesh models from available data, and
2. consistently couples vegetation parameterizations (porous media + wall functions) across momentum, turbulence, heat, humidity, and pollutant transport, suitable for coarse-grained city-scale domains.

**Hypotheses**
H1. Species-aware porous/wall-function parameterizations (drag, LAD, heat & moisture exchange, deposition) reproduce key micro-climate and dispersion metrics within acceptable errors vs field campaigns
H2. Using **semantic surfaces** with surface-specific wall parameters (roughness, thermal properties) improves RANS predictions in vegetated urban areas.
H3. An automated data to model to mesh pipeline scales to large urban areas while preserving enough fidelity (procedure based on *city4cfd* and *snappyHexMesh*)

**Objectives** (... Begin the path, completion isn't required)

1. Build an automatic pipeline from open data to **parametrized trees** (location/size/species) and computational mesh.
2. Implement vegetation effects via:
   • **Porous regions** (finite canopy height): momentum drag, turbulence source terms, heat & moisture sources, pollutant sinks;
   • **Vegetation boundaries** (no appreciable height): wall-based momentum/heat/mass/deposition fluxes.
3. Create a **species parameter set** (e.g. LAD, stomatal conductance (?), deposition velocities(?)).
5. Deliver a reproducible, scalable workflow for city grid generation and setup.
4. Validate against field campaigns (or high-fidelity simulations CFD benchmarks, see for example "CODASC: a database for the validation of street canyon dispersion models" by Christof Gromke and "Influence of trees on the dispersion of pollutants in an urban street canyon—Experimental investigation of the flow and concentration field" by Gromke and Ruck).

**Modelling (urban CFD setup)**

* Incompressible unsteady **RANS** with buoyancy (possibly with solution-driven Adaptive Mesh Refinement, driven by dispersion processes and/or turbulence model) .
* Multi-component mixture (dry air + water vapour).
* Heat transfer with radiation
* Pollutant transport with emissions and deposition.
* Vegetation modelling via porous regions and wall functions.
* Semantic surfaces for wall functions (roughness, thermal/ radiative properties, deposition-linked properties).

**Literature review**

1. Vegetation parameterizations: porous canopy (drag/LAD), turbulence production/dissipation, heat & moisture exchange, pollutant deposition.
2. City grid generation (from data to snappyHexMesh ready 3d models) and strategies to cope with huge and complex domains (e.g. implicit modelling of vegetation with porous regions / implicit modelling of buildings with immersed boundary layer technique (?) / Adaptive Mesh Refinement to adapt different scenarios/inflow boundary conditions).
3. Semantic surfaces & wall-function parameter choices.

Focus on vegetation-related modelling. Please delve deeply into how to represent vegetation effects using porosity models and/or wall functions across the governing equations (momentum, turbulence, pollutant transport, water vapour, and temperature). There’s no need to cover ABL configurations, or other omitted topics.

