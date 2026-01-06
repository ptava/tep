/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2020 ENERCON GmbH
    Copyright (C) 2020-2024 OpenCFD Ltd.
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

\*---------------------------------------------------------------------------*/

#include "myatmPlantCanopyUSource.H"
#include "addToRunTimeSelectionTable.H"
#include "fvmSup.H"

// * * * * * * * * * * * * * Static Member Functions * * * * * * * * * * * * //

namespace Foam
{
namespace fv
{
    defineTypeNameAndDebug(myatmPlantCanopyUSource, 0);
    addToRunTimeSelectionTable(option, myatmPlantCanopyUSource, dictionary);
}
}



// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::fv::myatmPlantCanopyUSource::myatmPlantCanopyUSource
(
    const word& sourceName,
    const word& modelType,
    const dictionary& dict,
    const fvMesh& mesh
)
:
    fv::cellSetOption(sourceName, modelType, dict, mesh),
    LAD_("LAD", dimensionSet(0, -1, 0, 0, 0, 0, 0), 0.0)
{
    read(dict);

    fieldNames_.resize(1, "U");

    fv::option::resetApplied();

    Log << "    Applying myatmPlantCanopyUSource to: " << fieldNames_[0] << endl;
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void Foam::fv::myatmPlantCanopyUSource::addSup
(
    fvMatrix<vector>& eqn,
    const label fieldi
)
{
    if (V_ < VSMALL)
    {
        return;
    }

    const volVectorField& U = eqn.psi();

    // (SP:Eq. 42), (BSG:Eq. 7)
    eqn -= fvm::Sp(Cd_*LAD_*mag(U), U);
}


void Foam::fv::myatmPlantCanopyUSource::addSup
(
    const volScalarField& rho,
    fvMatrix<vector>& eqn,
    const label fieldi
)
{
    if (V_ < VSMALL)
    {
        return;
    }

    const volVectorField& U = eqn.psi();

    eqn -= fvm::Sp(rho*Cd_*LAD_*mag(U), U);
}


void Foam::fv::myatmPlantCanopyUSource::addSup
(
    const volScalarField& alpha,
    const volScalarField& rho,
    fvMatrix<vector>& eqn,
    const label fieldi
)
{
    if (V_ < VSMALL)
    {
        return;
    }

    const volVectorField& U = eqn.psi();

    eqn -= fvm::Sp(alpha*rho*Cd_*LAD_*mag(U), U);
}


bool Foam::fv::myatmPlantCanopyUSource::read(const dictionary& dict)
{
    if (!fv::cellSetOption::read(dict))
    {
        return false;
    }

    // User provides only a scalar value for LAD, dimensions are fixed by
    // LAD_ construction
    scalar LADValue = LAD_.value();
    dict.lookup("LAD") >> LADValue;
    dict.lookup("Cd") >> Cd_;

    return true;
}


// ************************************************************************* //
