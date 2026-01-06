/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2020 ENERCON GmbH
    Copyright (C) 2020-2022 OpenCFD Ltd.
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

Class
    Foam::fv::myatmPlantCanopyUSource

Group
    grpFvOptionsSources

Description
    Applies sources on velocity (i.e. \c U) to incorporate effects
    of plant canopy for atmospheric boundary layer modelling.

    Corrections applied to:
    \verbatim
      U                | Velocity                               [m/s]
    \endverbatim

    Required fields:
    \verbatim
      U                | Velocity                               [m/s]
      Cd               | Plant canopy drag coefficient          [-]
      LAD              | Leaf area density                      [1/m]
    \endverbatim

    References:
    \verbatim
        Influence of forest (tag:SP):
            Sogachev, A., & Panferov, O. (2006).
            Modification of two-equation models to account for plant drag.
            Boundary-Layer Meteorology, 121(2), 229-266.
            DOI:10.1007/s10546-006-9073-5

        Governing equations (tag:BSG):
            Brozovsky, J., Simonsen, A., & Gaitani, N. (2021).
            Validation of a CFD model for the evaluation of urban microclimate
            at high latitudes: A case study in Trondheim, Norway.
            Building and Environment, 205, 108175.
            DOI:10.1016/j.buildenv.2021.108175
    \endverbatim

Usage
    Example by using \c constant/fvOptions:
    \verbatim
    myatmPlantCanopyUSource1
    {
        // Mandatory entries
        type         myatmPlantCanopyUSource;

        // Optional entries
        Cd           <word>;
        LAD          <word>;

        // Inherited entries
        ...
    }
    \endverbatim

    where the entries mean:
    \table
      Property | Description                      | Type   | Reqd  | Deflt
      type     | Type name: myatmPlantCanopyUSource | word   | yes   | -
      Cd       | Name of operand canopy drag coefficient field | word | no | Cd
      LAD      | Name of operand leaf area density field | word | no | LAD
    \endtable

    The inherited entries are elaborated in:
      - \link fvOption.H \endlink
      - \link cellSetOption.H \endlink

SourceFiles
    myatmPlantCanopyUSource.C
    myatmPlantCanopyUSourceTemplates.C

\*---------------------------------------------------------------------------*/

#ifndef fv_myatmPlantCanopyUSource_H
#define fv_myatmPlantCanopyUSource_H

#include "cellSetOption.H"
#include "fvMatrices.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace fv
{

/*---------------------------------------------------------------------------*\
                    Class myatmPlantCanopyUSource Declaration
\*---------------------------------------------------------------------------*/

class myatmPlantCanopyUSource
:
    public fv::cellSetOption
{
    // Private Data

        dimensionedScalar   LAD_;
        scalar   Cd_;


public:

    //- Runtime type information
    TypeName("myatmPlantCanopyUSource");


    // Constructors

        //- Construct from explicit source name and mesh
        myatmPlantCanopyUSource
        (
            const word& sourceName,
            const word& modelType,
            const dictionary& dict,
            const fvMesh& mesh
        );

        //- No copy construct
        myatmPlantCanopyUSource(const myatmPlantCanopyUSource&) = delete;

        //- No copy assignment
        void operator=(const myatmPlantCanopyUSource&) = delete;


    // Member Functions

        //- Add explicit contribution to momentum equation
        virtual void addSup
        (
            fvMatrix<vector>& eqn,
            const label fieldi
        );

        //- Add explicit contribution to compressible momentum equation
        virtual void addSup
        (
            const volScalarField& rho,
            fvMatrix<vector>& eqn,
            const label fieldi
        );

        //- Add explicit contribution to phase momentum equation
        virtual void addSup
        (
            const volScalarField& alpha,
            const volScalarField& rho,
            fvMatrix<vector>& eqn,
            const label fieldi
        );

        //- Read source dictionary
        virtual bool read(const dictionary& dict);
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace fv
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
