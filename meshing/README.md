## From 3D models to computational domain

Tools: blockMesh + snappyHexMesh + topoSet

### How we build computational grid for cities digital twins?

We aim to provide the most automated and scalable solution to build the computational grid of cities digital twins. The procedure ends up with an unstructured grid with cubic cells in the region of interest.

Starting point: 3d models of cities' features (buildings, terrain, vegetation, etc) in *stl* format. Coupling together all geometries (plus external domain boundaries) we should end up with a watertight model without any gaps or intersections. Those objects which effect is accounted via porosity models are an exception (e.g., vegetation).

Remember: all of this should be automated as much as possible with all user-defined parameters stored in configuration files (e.g., OpenFOAM dictionary or other better format)

Example of `userDict`
```
refinement
{
    // refinement for wake region
    refinementRegion1
    {
        min_x   -0.9;
        max_x   1.4;
        min_y   -0.9;
        max_y   1.4;
        min_z   0.0;
        max_z   0.9;
    }

    ...

}

background
{
    // Length of cubic cells
    base_length  0.05;

    // Domain limit
    min_x   -1.4;
    max_x   3.0;
    min_y   -1.4;
    max_y   3.0;
    min_z   0;
    max_z   2.8; 

    // Lengths
    length_x   #calc " ${max_x} - ${min_x} ";
    length_y   #calc " ${max_y} - ${min_y} ";
    length_z   #calc " ${max_z} - ${min_z} ";

    // Number of cells in each direction
    n_cells_x   #calc " ${length_x} / ${base_length} ";
    n_cells_y   #calc " ${length_y} / ${base_length} ";
    n_cells_z   #calc " ${length_z} / ${base_length} ";

    ...
}

...

```

To recall this file within OpenFOAM files we can include it at the beginning of the file:
```
#include "userDict"
```

Ultimately, the background grid should be defined exploiting o-grid technique in order to have two concentric annular regions outside the "uniform cubic cells region" in the center of the domain. Here a schematic representation of the desired grid structure (top view):

```
                              OOOOOO
                     OOOOOO           OOOOOO
                 OOO                        OOO
              OOO                              OOO
            +O                                    O+
          OO  \               oooo               /  OO
         OO     \      oooo          oooo      /     OO
        O         \ ooo                  ooo /         O
       O          o+\                      /+o          O
      OO         oo   +------------------+   oo         OO
      O         oo    | \              / |    oo         O
     OO        oo     |   \          /   |     oo        OO
     O         o      |     \      /     |      o         O
     O         o      |         X        |      o         O
     O         o      |     /      \     |      o         O
     OO        oo     |   /          \   |     oo        OO
      O         oo    | /              \ |    oo         O
      OO         oo   +------------------+   oo         OO
       O          o+/                      \+o          O
        O         / ooo                  ooo \         O
         OO     /      oooo          oooo      \     OO
          OO  /               oooo               \  OO
            +O                                    O+
              OOO                              OOO
                 OOO                        OOO
                     OOOOOO          OOOOOO
                              OOOOOO

```


## Steps
1. Prepare background mesh
2. Prepare stl files
3. Extract surface features
4. Fill out snappyHexMeshDict and run snappyHexMesh (castellatedMesh is enough for now)
5. Fill out the boundary conditions
6. Fill out domain properties


### 1. Prepare background mesh

Fill out blockMeshDict file located in the *system* folder: define a single block representing the entire extent of the computational domain; 

BlockMesh usage:
```
  The ordering of vertex and face labels within a block as shown below.
  For the local vertex numbering in the sequence 0 to 7:
    Faces 0, 1 (x-direction) are left, right.
    Faces 2, 3 (y-direction) are front, back.
    Faces 4, 5 (z-direction) are bottom, top.

                        7 ---- 6
                 f5     |\     :\     f3
                 |      | 4 ---- 5     \
                 |      3.|....2 |      \
                 |       \|     \|      f2
                 f4       0 ---- 1
    Y  Z
     \ |                f0 ------ f1
      \|
       o--- X
```

First attempts will include a simple background grid with a single block with uniform cells

Notes: 
- use Z as vertical direction;
- `terrain` surface must extend beyond this block in all directions.
- number of cubic cells computed base on user-defined size (e.g., 2 m)
- define uniform cells grading

The final result should consider an o-grid technique with single rounded-square-based uniform grid in the center (encompassing the area of interest where we model everything in detail) two annular regions (one with uniform spacing where we've the real terrain) and another one after the first one with a smooth grading to reach the outer boundary of the domain with a flat terrain. The domain limits are a cylinder lateral face. These subsequent annular regions are made of four blocks each. The user should be able to define the background grid by only providing four parameters:
- square size of the inner region
- radial width of the first annular region with uniform spacing
- radial width of the second annular region with grading
- cell size in inner region
- height of the domain

! Overall cells count can be controlled by defining last cell size for lateral boundary and height of the domain (or with cell-expansion ration constraint)


BlockMeshDict example:
```
vertices
(
      ( $/background/min_x $/background/min_y $/background/min_z )
      ( $/background/max_x $/background/min_y $/background/min_z )
      ( $/background/max_x $/background/max_y $/background/min_z )
      ( $/background/min_x $/background/max_y $/background/min_z )
      ( $/background/min_x $/background/min_y $/background/max_z )
      ( $/background/max_x $/background/min_y $/background/max_z )
      ( $/background/max_x $/background/max_y $/background/max_z )
      ( $/background/min_x $/background/max_y $/background/max_z )


);

blocks
(
    hex (0 1 2 3 4 5 6 7)(
        $/background/n_cells_x
        $/background/n_cells_y
        $/background/n_cells_z
    ) 
    simpleGrading ( 1 1 1 )
);

edges
(
);

boundary
(
    inlet
    {
        type patch;
        faces
        (
            (0 4 7 3)
        );
    }

    outlet
    {
        type patch;
        faces
        (
            (1 2 6 5)
            (3 7 6 2)
        );
    }

    top
    {
        type patch;
        faces
        (
            (4 5 6 7)
        );
    }
  
    ground
    {
        type wall;
        faces
        (
            (0 3 2 1)
        );
    }

     
    lateral1
    {
        type symmetry;
        faces
        (
            (1 5 4 0)
        );
    }
     
);
```


### 2. Prepare stl files
The stl files should be placed in the *constant/triSurface* or *constant/geometry* folder depending on the OpenFOAM version. The files should be named according to the features they represent, e.g., buildings.stl, terrain.stl, vegetation.stl, etc., and should be in ASCII format. The solid object name at the beginning and the end of the stl file should match the file name.

### 3. Extract surface features
To improve the snapping process of snappyHexMesh, we extract surface features from the stl files using the surfaceFeatures command. This requires the definition of the surfaceFeaturesDict file located in the *system* folder. This command generates edge files for each stl surface, which are then used during the snapping process to better capture the geometry of the surfaces.

Example of surfaceFeaturesDict:
```
terrain.stl
{
    #includeEtc "caseDicts/surface/surfaceFeatureExtractDict.cfg"
}
buildings.stl
{
    #includeEtc "caseDicts/surface/surfaceFeatureExtractDict.cfg"
}
...
```


### 4. Fill out snappyHexMeshDict
Prepare snappyHexMeshDict file located in the *system* folder. The main sections to be filled out are: geometry, castellatedMeshControls, and snapControls.

#### geometry

Inside geometry dictionary we define all the geometries required. Here the geometries are both the surfaces used for boundaries definition and the volumes used for cell refinement or region models.

```
geometry
{
    buildings.stl
    {
        type triSurfaceMesh;
        name buildings;
    }
    terrain.stl
    {
        type triSurfaceMesh;
        name terrain;
    }
    vegetation.stl
    {
        type triSurfaceMesh;
        name vegetation;
    }
    refinementRegion1
    {
        type searchableBox;
        min  ($.refinement.refinementRegion1.min_x $.refinement.refinementRegion1.min_y $.refinement.refinementRegion1.min_z);
        max  ($.refinement.refinementRegion1.max_x $.refinement.refinementRegion1.max_y $.refinement.refinementRegion1.max_z);
    }
    ...
}
```

#### castellatedMeshControls
Other then the standard parameters, we need to define:
- features sub-dictionary:
```
    features
    (
        { file  "terrain.eMesh"; level 3; }
        { file  "buildings.eMesh"; level 3; }
    );
```

- refinementSurfaces sub-dictionary:

```
    refinementSurfaces
    {
        buildings
        {
            level (3 5);
            patchInfo
            {
                type wall;
                inGroups (group_example);
            }
        }
        terrain
        {
            level (2 4);
            patchInfo
            {
                type wall;
                inGroups (group_example);
            }
        }
        trees
        {
            level       (5 5);
            faceZone    treesZone;
            cellZone    treesZone;
            cellZoneInside  inside;
        }

    }
```

- refinementRegions sub-dictionary:
```
    refinementRegions
    {
        refinementRegion1
        {
            mode inside;
            levels ((1E15 3));
        }
    }
```

- locationInMesh parameter:
```
    locationInMesh (0 0 15);   // inside the fluid domain away from cells faces
```

#### snapControls
Example of snapControls sub-dictionary:
```
    nSmoothPatch 5;
    tolerance 0.8;
    nSolveIter 30;
    nRelaxIter 5;
    nFeatureSnapIter 10;
    implicitFeatureSnap false;
    explicitFeatureSnap true;
    multiRegionFeatureSnap false;
```

### 5. Fill out the boundary conditions
After running snappyHexMesh, we need to define the boundary conditions for each patch in the constant/polyMesh/boundary file. The definition of the boundary conditions files will be automatic based on user defined values in the configuration file (e.g., userDict).

The file is structured as follows:
```
/*--------------------------------*- C++ -*----------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
    \\  /    A nd           | Version:  dev
     \\/     M anipulation  |
\*---------------------------------------------------------------------------*/
FoamFile
{
    version     2.0;
    format      ascii;
    class       volVectorField;
    location    "0";
    object      U;
}
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

dimensions      [0 1 -1 0 0 0 0];

internalField   uniform (0 0 0);

boundaryField
{
    building
    {
        type            noSlip;
    }
    terrain
    {
        type            noSlip;
    }
    ...
}
// ************************************************************************* //
```

The quantities of interest are going to be the following: U, p, k, omega, nut, and species-related quantities for the concentration of gaseous substances in air (including pollutants and humidity). For each file and boundary we need a table of all parameters needed.

With reference to OpenFOAM (ESI version) `atmosphericModels` library.

We need to associate to each boundary-quantity pair a specific **type** and its parameters, with a small script that goes from the configuration file to boundary files. For example each wall boundary will have a `noSlip` type for U, a `zeroGradient` type for p, and for omega `atmWallFunction` with additional parameter surface roughness length `z0` specific to each boundary.

## 6. Fill out domain properties
Finally, we need to define the domain properties in the constant folder. This includes defining the turbulence model, physical properties of the fluid (air), and any other relevant parameters for the simulation. These are associated with `turbulenceProperties`, `thermophysicalProperties`, `fvOptions`, and `g` files. We're dealing with incompressible flow with heat transfer and multi-component mixture.

Porosity model for vegetation need to be included in `fvOptions` file. Available models in OpenFOAM to account for the effect of porous region into momentum equation are: `explicitPorositySource` with `DarcyForchheimer`, `powerLaw`, and `powerLawLopesDaCosta`, or available porosity model based on Leaf Area Density (LAD) such as `atmPlantCanopyUSource`. To account for the effect of vegetation into other transport equations (e.g., temperature and turbulence) we only have `atmPlantCanopyTSource` and `atmPlantCanopyTurbSource`. To set up associated parameters (e.g., LAD) we need to rely on `setFields` taking user-defined values from configuration file. These models rely on the presence of a `cellZone` which is a named region of the computational domain that can be defined directly inside snappyHexMeshDict (as in above-mentioned example) or it can be done afterwards with topoSet or createZone utilities, depending on OpenFOAM version.

It is better to do both: refinement with zone definition inside snappyHexMeshDict plus topoSet afterwards.

To visualize created zones use `foamToVTK` utility.

# Examples
1. `test_single_tree`: take a small portion of domain on which we can test grid generation from semanthic 3d models
    - terrain characterised by multiple named surfaces
    - single tree with crown and trunk as separate objects
    ==> generate grid with two named regions for tree crown and trunk
2. `test_overlapping`: take the same tree and use surfaceTransformPoints utility to create a copy with overlapping crown, translate also the trunk and put it in the same file for simplicity instead of creating a second trunk file
    ==> generate grid with two named regions for tree crowns and single region for trunks
    - make the domain larger and suitable for ABL-like simualtion
    ==> use it as test case to check for a proper modelling of overlapping crowns via LAD field (look at tutorials in: `verificationAndValidation/atmosphericModels` for what's available in OpenFOAM for ABL simulations with vegetation)
3. `test_complete`: let's take a Firenze city district and model buildings, terrain, and vegetation
