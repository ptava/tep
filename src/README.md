## OpenFOAM source code

This folder contains the additional source code required to run the simulations. Modified libraries are:
- `atmosphericModels`

Additional code associated with:
- modified *fvOption* for vegetation effects on momentum equation (`myatmPlantCanopyUSource`)

Description:
- `myatmPlantCanopyUSource`: *LAD* and *Cd* are now provided by the user as specific scalar values for each region (*cellZone*) of interest. This change is done to account for possible overlapping vegetation layers with different characteristics.

Tested with OpenFOAM v2412
