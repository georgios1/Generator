//____________________________________________________________________________
/*
 Copyright (c) 2003-2020, The GENIE Collaboration
 For the full text of the license visit http://copyright.genie-mc.org

*/
//____________________________________________________________________________

#include "Framework/Messenger/Messenger.h"
#include "Framework/ParticleData/PDGCodes.h"
#include "Framework/ParticleData/PDGUtils.h"
#include "Framework/ParticleData/PDGLibrary.h"
#include "Framework/Utils/PrintUtils.h"
#include "Physics/NeutralHeavyLepton/NHLDecayUtils.h"

using namespace genie;

//____________________________________________________________________________
string genie::utils::nhl::AsString(NHLDecayMode_t nhldm)
{
  switch(nhldm) {

  case (kNHLDcyNull):
    return "Invalid NHL decay mode!";
    break;

  case(kNHLDcEENu):
  return "N -> nu e+ e-";

  case(kNHLDcMuENu):
  return "N -> nu e+ mu- ";

  case(kNHLDcEMuNu): 
  return "N -> nu mu+ e-";

  case(kNHLDcMuMuNu): 
  return "N -> nu mu+ mu-"; 
  
  case(kNHLDcPiE): 
  return "N -> pi+ e-"; 
  
  case(kNHLDcPiMu):
  return "N -> mu- pi+";
 
  case(kNHLDcKE): 
  return "N -> e- K+"; 
 
  case(kNHLDcKMu):
  return "N -> mu- K+";
  
  case(kNHLDcPi0Nu):
  return "N -> nu Pi0";

  case(kNHLDcRho0Nu):
  return "N -> nu Rho0" ; 

  case(kNHLDcRhoE):
  return "N -> e- Rho+"; 

  case(kNHLDcOmegaNu): 
  return "N -> nu Omega" ; 

  case(kNHLDcRhoMu):
  return "N -> mu- Rho+" ; 
 
  case(kNHLDcPhiNu):
  return "N -> nu Phi"; 

  case(kNHLDcTauENu): 
  return "N -> nu e+ tau-";
 
  case(kNHLDcDE): 
  return "N-> D+ e-"; 

  case(kNHLDcTauMuNu): 
  return "N -> tau- mu+ nu";

  case(kNHLDcPiTau): 
  return "N -> Pi+ tau-";  

  }
  return "Invalid NHL decay mode!";
}
//____________________________________________________________________________
bool genie::utils::nhl::IsKinematicallyAllowed(NHLDecayMode_t nhldm, double M)
{
// Check the input mass of the NHL (M) against the sum of the masses of the
// particles to be produced in the input decay

  PDGCodeList decay_products = DecayProductList(nhldm);

  PDGLibrary * pdglib = PDGLibrary::Instance();

  double Msum = 0.;
  PDGCodeList::const_iterator it = decay_products.begin();
  for ( ; it != decay_products.end(); ++it)
  {
    int pdg_code = *it;
    TParticlePDG * p = pdglib->Find(pdg_code);
    if(p) {
       Msum += p->Mass();
    } else {
       LOG("NHL", pERROR)
        << "Decay list includes particle with unrecognised PDG code: "
        << pdg_code;
    }
  }

  return (M > Msum);
}
//____________________________________________________________________________
PDGCodeList genie::utils::nhl::DecayProductList(NHLDecayMode_t nhldm)
{
  bool allow_duplicate = true;
  PDGCodeList decay_products(allow_duplicate);

  switch(nhldm) {

  // Specify the final state particles in each NHL decay channel,
  // adding sections that look like
  //
  //  case (kNHLDcy...):
  //     decay_products.push_back(kPdgPositron);
  //     decay_products.push_back(kPdgElectron);
  //     decay_products.push_back(... some other particle PDG code);
  //     break;
  //

  case(kNHLDcEENu):
    decay_products.push_back(kPdgNuE);
    decay_products.push_back(kPdgPositron);
    decay_products.push_back(kPdgElectron);
    break;

  case(kNHLDcMuENu):
    decay_products.push_back(kPdgNuE);
    decay_products.push_back(kPdgPositron);
    decay_products.push_back(kPdgMuon);
    break;

  case(kNHLDcEMuNu): 
    decay_products.push_back(kPdgNuMu);
    decay_products.push_back(kPdgAntiMuon);
    decay_products.push_back(kPdgElectron);
    break ;
  case(kNHLDcMuMuNu):
    decay_products.push_back(kPdgNuMu);
    decay_products.push_back(kPdgAntiMuon);
    decay_products.push_back(kPdgMuon);
    break ;
  
  case(kNHLDcPiE): 
    decay_products.push_back(kPdgPiP);
    decay_products.push_back(kPdgElectron);
    break;

  case(kNHLDcPiMu): 
    decay_products.push_back(kPdgPiP);
    decay_products.push_back(kPdgMuon);
    break;

  case(kNHLDcKE): 
    decay_products.push_back(kPdgKP);
    decay_products.push_back(kPdgElectron);
    break;
 
  case(kNHLDcKMu):
    decay_products.push_back(kPdgKM);
    decay_products.push_back(kPdgMuon);
    break;

  case(kNHLDcRho0Nu):
    decay_products.push_back(kPdgRho0);
    decay_products.push_back(kPdgNuMu);
    break; 

  case(kNHLDcRhoE):
    decay_products.push_back(kPdgRhoP);
    decay_products.push_back(kPdgElectron);
    break; 

  case(kNHLDcOmegaNu):
    decay_products.push_back(kPdgomega);
    decay_products.push_back(kPdgNuMu);
    break; 
  
  case(kNHLDcRhoMu):
    decay_products.push_back(kPdgRhoP);
    decay_products.push_back(kPdgMuon);
    break; 

  case(kNHLDcPhiNu):
    decay_products.push_back(kPdgPhi);
    decay_products.push_back(kPdgNuMu);
    break; 

  case(kNHLDcTauENu):
    decay_products.push_back(kPdgTau);
    decay_products.push_back(kPdgElectron);
    decay_products.push_back(kPdgNuE);
    break; 

  case(kNHLDcDE):
    decay_products.push_back(kPdgDP);
    decay_products.push_back(kPdgElectron);
    break; 

  case(kNHLDcTauMuNu):
    decay_products.push_back(kPdgTau);
    decay_products.push_back(kPdgMuon);
    decay_products.push_back(kPdgNuMu);
    break; 

  case(kNHLDcPiTau):
    decay_products.push_back(kPdgPiP);
    decay_products.push_back(kPdgTau);
    break; 
 
  default :
    break;
  }

  return decay_products;
}
//____________________________________________________________________________
