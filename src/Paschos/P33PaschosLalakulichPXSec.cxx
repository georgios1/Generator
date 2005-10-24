//____________________________________________________________________________
/*!

\class    genie::P33PaschosLalakulichPXSec

\brief    Double differential resonance cross section d^2xsec / dQ^2 dW
          for P33 according to the Paschos, Lalakuich model.

          Is a concrete implementation of the XSecAlgorithmI interface.

\ref      O.Lalakulich and E.A.Paschos, Resonance Production by Neutrinos:
          I. J=3/2 Resonances, hep-ph/0501109

\author   This class is based on code written by the model authors (Olga
          Lalakulich, 17.02.2005). The code was modified to fit into the
          GENIE framework by Costas Andreopoulos.

\created  February 22, 2005

*/
//____________________________________________________________________________

#include <string>

#include <TMath.h>

#include "BaryonResonance/BaryonResonance.h"
#include "BaryonResonance/BaryonResParams.h"
#include "BaryonResonance/BaryonResDataSetI.h"
#include "Conventions/Constants.h"
#include "Conventions/RefFrame.h"
#include "Messenger/Messenger.h"
#include "Paschos/P33PaschosLalakulichPXSec.h"
#include "Utils/KineUtils.h"
#include "Utils/MathUtils.h"
#include "Utils/Range1.h"

using std::string;

using namespace genie;
using namespace genie::constants;

//____________________________________________________________________________
P33PaschosLalakulichPXSec::P33PaschosLalakulichPXSec() :
XSecAlgorithmI("genie::P33PaschosLalakulichPXSec")
{

}
//____________________________________________________________________________
P33PaschosLalakulichPXSec::P33PaschosLalakulichPXSec(string config) :
XSecAlgorithmI("genie::P33PaschosLalakulichPXSec", config)
{

}
//____________________________________________________________________________
P33PaschosLalakulichPXSec::~P33PaschosLalakulichPXSec()
{

}
//____________________________________________________________________________
double P33PaschosLalakulichPXSec::XSec(const Interaction * interaction) const
{
  LOG("Paschos", pDEBUG) << *fConfig;

  //-- Get initial state and kinematic variables

  const InitialState & init_state = interaction -> GetInitialState();

  TLorentzVector * nu_p4 = init_state.GetProbeP4(kRfStruckNucAtRest);

  double E = nu_p4->Energy();

  delete nu_p4;

  const ScatteringParams & sc_params = interaction -> GetScatteringParams();

  double E2   = TMath::Power(E,2);
  double Q2   = sc_params.Q2();
  double W    = sc_params.W();

  //-- Commonly used masses

  double MN   = kNucleonMass;
  double MN2  = kNucleonMass_2;
  double Mmu2 = kMuonMass_2;
  double Mpi2 = kPionMass_2;

  //-- Compute kinematically allowed W, Q2 range
  //   Return 0 if not valid kinematics
  //   Here I am using the standard GENIE kinematic limit functions from the
  //   utils::kinematics phase space. These differ from the expressions in the
  //   Paschos-Lalakulich paper that are inverted and W limits are expressed
  //   as a function of Q^2. This should not make any difference.

  Range1D_t rW  = utils::kinematics::WRange(interaction);
  Range1D_t rQ2 = utils::kinematics::Q2Range_W(interaction);

  LOG("Paschos", pDEBUG) << "\n Physical W range: "
                         << "["<< rW.min   << ", " << rW.max  << "] GeV";
  LOG("Paschos", pDEBUG) << "\n Physical Q2 range: "
                         << "[" << rQ2.min << ", " << rQ2.max << "] GeV^2";
  bool is_within_limits =
               utils::math::IsWithinLimits(W, rW) &&
                                     utils::math::IsWithinLimits(Q2, rQ2);

  if( !is_within_limits ) return 0.;

  //-- P33(1232) information

  const Algorithm * algbase = this->SubAlg("baryon-res-alg-name", "baryon-res-param-set");

  const BaryonResDataSetI * resonance_data =
                        dynamic_cast<const BaryonResDataSetI *> (algbase);

  BaryonResParams res_params;

  res_params.SetDataSet(resonance_data);
  res_params.RetrieveData(kP33_1232);

  double MR   = res_params.Mass();
  double MR2  = TMath::Power(MR,2);
  double MR3  = TMath::Power(MR,3);

  double Gamma_R0 = res_params.Width();

  //-- MA,MV and coupling consts

  double MA     = this->MA();
  double MV     = this->MV();
  double MA2    = TMath::Power( MA, 2 );
  double MV2    = TMath::Power( MV, 2 );
  double ftmp1a = TMath::Power( 1 + Q2/MA2, 2 );
  double ftmp1v = TMath::Power( 1 + Q2/MA2, 2 );
  double ftmp2a = 1 + Q2/3/MA2;
  double ftmp2v = 1 + Q2/4/MV2;

  double f3A = kPlRes_f3_P1232_A/ftmp1a/ftmp2a;
  double f4A = kPlRes_f4_P1232_A/ftmp1a/ftmp2a;
  double f5A = kPlRes_f5_P1232_A/ftmp1a/ftmp2a;
  double f6A = kPlRes_f6_P1232_A/ftmp1a/ftmp2a/(Q2+Mpi2);
  double f3V = kPlRes_f3_P1232_V/ftmp1v/ftmp2v;
  double f4V = kPlRes_f4_P1232_V/ftmp1v/ftmp2v/W;
  double f5V = kPlRes_f5_P1232_V/ftmp1v/ftmp2v;

  // Most of the following params were defined in the original code but
  // left unused. Here, they are commented out to suppress gcc warnings

  //double f3V4V = f3V*f4V; // f3V..
  //double f3V5V = f3V*f5V;
  //double f3V3A = f3V*f3A;
  double f3V4A = f3V*f4A;
  double f3V5A = f3V*f5A;
  //double f3V6A = f3V*f6A;
  //double f4V5V = f4V*f5V; // f4V..
  //double f4V3A = f4V*f3A;
  double f4V4A = f4V*f4A;
  double f4V5A = f4V*f5A;
  //double f4V6A = f4V*f6A;
  //double f5V3A = f5V*f3A; // f5V..
  //double f5V4A = f5V*f4A;
  //double f5V5A = f5V*f5A;
  //double f5V6A = f5V*f6A;
  //double f3A4A = f3A*f4A; // f3A..
  //double f3A5A = f3A*f5A;
  //double f3A6A = f3A*f6A;
  //double f4A5A = f4A*f5A; // f4A..
  //double f4A6A = f4A*f6A;
  //double f5A6A = f5A*f6A;

  double f3A2 = TMath::Power( f3A, 2 );
  double f4A2 = TMath::Power( f4A, 2 );
  double f5A2 = TMath::Power( f5A, 2 );
  double f6A2 = TMath::Power( f6A, 2 );
  double f3V2 = TMath::Power( f3V, 2 );
  double f4V2 = TMath::Power( f4V, 2 );
  double f5V2 = TMath::Power( f5V, 2 );

  //-- move these running gamma definitions into GENIE's breit-wigner
  //   functions so that they can be directly used here

  // model of the running Gamma from Paschos [default]
  double Gamma_R=Gamma_R0*pow((this->PPiStar(W)/this->PPiStar(MR)),3);

  // check for other option
  if ( fConfig->Exists("running-gamma") ) {

     string gamma_model = fConfig->GetString("running-gamma");

     if ( gamma_model.find("Hagiwara") != string::npos )
     {
         // model of the running Gamma from Hagiwara et. al.
         Gamma_R = Gamma_R0*MR/W*pow((this->PPiStar(W)/this->PPiStar(MR)),1);
     } else
     if ( gamma_model.find("Galster") != string::npos )
     {
         // model of the running Gamma similar to Galster-1972
         double gtmp1 = TMath::Power( this->PPiStar(W) / this->PPiStar(MR), 3);
         double gtmp2 = TMath::Power( this->PPiStar(W) / this->PPiStar(MR), 2);
         Gamma_R = Gamma_R0*gtmp1/(1+gtmp2);
     }
  }
  double Breit_Wigner = TMath::Power(W*W-MR2,2) + MR2 * TMath::Power(Gamma_R,2);

  //-- Include Pauli suppression [if the option was turned on by the user]

  bool add_pauli_correction =
           fConfig->Exists("turn-on-pauli-suppression") ?
                         fConfig->GetBool("turn-on-pauli-suppression") : false;
  double pauli = 1.;
  if(add_pauli_correction) pauli = this->Pauli(Q2,W);

  //-- Kinematic variables

  double nu    = this->Nu(Q2,W);
  double pq  = MN*nu;
  double qk  = -(Q2+Mmu2)/2.;
  double pk  = MN*E;

  double pq2 = TMath::Power(pq,2);
  double pq3 = TMath::Power(pq,3);
  double Q4  = TMath::Power(Q2,2);

  // defined in the original code but left unused
  // commented out to suppress gcc warning
  //double qW  = MN*nu-Q2;
  //double pW  = (W*W+Q2+MN2)/2.;

  //  W1 -> W5 from Lalakulich-Paschos paper

  double W1=0, W2=0, W3=0, W4=0, W5=0;

  W1 = 3.*(2*f5A2*MN2*MR2+2*f5A2*MN*MR3+2*f3A*f5A*MN2*MR*pq+2*f5A2*MR2*pq
       +4*f3A*f5A*MN*MR2*pq+4*f4A*f5A*MN2*MR2*pq+2*f3A*f5A*MR3*pq
       +4*f4A*f5A*MN*MR3*pq+2*f3A2*MN2*pq2+2*f3V2*MN2*pq2+2*f3A*f5A*MR*pq2
       +2*f3A*f4A*MN2*MR*pq2+2*f3V*f4V*MN2*MR*pq2+2*f3V*f5V*MN2*MR*pq2
       +2*f3A2*MR2*pq2+2*f3V2*MR2*pq2+4*f4A*f5A*MR2*pq2+4*f3A*f4A*MN*MR2*pq2
       -4*f3V*f4V*MN*MR2*pq2-4*f3V*f5V*MN*MR2*pq2+2*f4A2*MN2*MR2*pq2+2*f4V2*MN2*MR2*pq2
       +4*f4V*f5V*MN2*MR2*pq2+2*f5V2*MN2*MR2*pq2+2*f3A*f4A*MR3*pq2+2*f3V*f4V*MR3*pq2
       +2*f3V*f5V*MR3*pq2+2*f4A2*MN*MR3*pq2-2*f4V2*MN*MR3*pq2-4*f4V*f5V*MN*MR3*pq2
       -2*f5V2*MN*MR3*pq2+2*f3A2*pq3+2*f3V2*pq3+2*f3A*f4A*MR*pq3+2*f3V*f4V*MR*pq3
       +2*f3V*f5V*MR*pq3+2*f4A2*MR2*pq3+2*f4V2*MR2*pq3+4*f4V*f5V*MR2*pq3+2*f5V2*MR2*pq3
       -2*f3A*f5A*MN2*MR*Q2-4*f3A*f5A*MN*MR2*Q2+2*f3A2*MN2*MR2*Q2
       +2*f3V2*MN2*MR2*Q2-4*f4A*f5A*MN2*MR2*Q2-2*f3A2*MN*MR3*Q2+2*f3V2*MN*MR3*Q2
       -4*f4A*f5A*MN*MR3*Q2-4*f3A2*MN2*pq*Q2-4*f3V2*MN2*pq*Q2-2*f3A*f5A*MR*pq*Q2
       -4*f3A*f4A*MN2*MR*pq*Q2-4*f3V*f4V*MN2*MR*pq*Q2-2*f3V*f5V*MN2*MR*pq*Q2
       -4*f4A*f5A*MR2*pq*Q2-8*f3A*f4A*MN*MR2*pq*Q2+8*f3V*f4V*MN*MR2*pq*Q2
       +4*f3V*f5V*MN*MR2*pq*Q2-4*f4A2*MN2*MR2*pq*Q2-4*f4V2*MN2*MR2*pq*Q2
       -4*f4V*f5V*MN2*MR2*pq*Q2-2*f3A*f4A*MR3*pq*Q2-2*f3V*f4V*MR3*pq*Q2
       -4*f4A2*MN*MR3*pq*Q2+4*f4V2*MN*MR3*pq*Q2+4*f4V*f5V*MN*MR3*pq*Q2
       -4*f3A2*pq2*Q2-4*f3V2*pq2*Q2-4*f3A*f4A*MR*pq2*Q2-4*f3V*f4V*MR*pq2*Q2
       -2*f3V*f5V*MR*pq2*Q2-4*f4A2*MR2*pq2*Q2-4*f4V2*MR2*pq2*Q2-4*f4V*f5V*MR2*pq2*Q2
       +2*f3A2*MN2*Q4+2*f3V2*MN2*Q4+2*f3A*f4A*MN2*MR*Q4+2*f3V*f4V*MN2*MR*Q4
       +4*f3A*f4A*MN*MR2*Q4-4*f3V*f4V*MN*MR2*Q4+2*f4A2*MN2*MR2*Q4+2*f4V2*MN2*MR2*Q4
       +2*f4A2*MN*MR3*Q4-2*f4V2*MN*MR3*Q4+2*f3A2*pq*Q4+2*f3V2*pq*Q4+2*f3A*f4A*MR*pq*Q4
       +2*f3V*f4V*MR*pq*Q4+2*f4A2*MR2*pq*Q4+2*f4V2*MR2*pq*Q4)/(3.*MR2);

  W2 = 3.*(2*(f5A2*MN2
       +f5A2*MN*MR+f5A2*pq+f3A2*MN2*Q2+f3V2*MN2*Q2+f3A*f5A*MR*Q2+f3A*f4A*MN2*MR*Q2
       +f3V*f4V*MN2*MR*Q2+f3V*f5V*MN2*MR*Q2+f3A2*MR2*Q2+f3V2*MR2*Q2
       +2*f3A*f4A*MN*MR2*Q2-2*f3V*f4V*MN*MR2*Q2-2*f3V*f5V*MN*MR2*Q2
       +f4A2*MN2*MR2*Q2+f4V2*MN2*MR2*Q2+2*f4V*f5V*MN2*MR2*Q2+f5V2*MN2*MR2*Q2+f3A*f4A*MR3*Q2
       +f3V*f4V*MR3*Q2+f3V*f5V*MR3*Q2+f4A2*MN*MR3*Q2-f4V2*MN*MR3*Q2
       -2*f4V*f5V*MN*MR3*Q2-f5V2*MN*MR3*Q2+f3A2*pq*Q2+f3V2*pq*Q2+f3A*f4A*MR*pq*Q2
       +f3V*f4V*MR*pq*Q2+f3V*f5V*MR*pq*Q2+f4A2*MR2*pq*Q2+f4V2*MR2*pq*Q2
       +2*f4V*f5V*MR2*pq*Q2+f5V2*MR2*pq*Q2+f5V2*MN2*Q4+f3V*f5V*MR*Q4
       -f5V2*MN*MR*Q4+f5V2*pq*Q4))/(3.*MR2);

  W3 = 3.*((f3V4A*(Q2-pq)-f3V5A)*(2*MR2+2*MN*MR+Q2-pq)*4./3./MR
       -(Q2-pq)*(f4V4A*(Q2-pq)-f4V5A)*4./3.);


  W4 = 3.*(2*(f5A2*MN2+f5A2*MN*MR+f3A*f5A*MN2*MR
       +2*f3A*f5A*MN*MR2-f3A2*MN2*MR2-f3V2*MN2*MR2+2*f4A*f5A*MN2*MR2-2*f5A*f6A*MN2*MR2
       +f3A2*MN*MR3-f3V2*MN*MR3+2*f4A*f5A*MN*MR3-2*f5A*f6A*MN*MR3+f5A2*pq
       +2*f3A2*MN2*pq+2*f3V2*MN2*pq+2*f5A*f6A*MN2*pq+2*f3A*f5A*MR*pq
       +2*f5A*f6A*MN*MR*pq+2*f3A*f4A*MN2*MR*pq+2*f3V*f4V*MN2*MR*pq
       +f3V*f5V*MN2*MR*pq-f3A*f6A*MN2*MR*pq+2*f4A*f5A*MR2*pq
       -2*f5A*f6A*MR2*pq+4*f3A*f4A*MN*MR2*pq-4*f3V*f4V*MN*MR2*pq
       -2*f3V*f5V*MN*MR2*pq-2*f3A*f6A*MN*MR2*pq+2*f4A2*MN2*MR2*pq
       +2*f4V2*MN2*MR2*pq+2*f4V*f5V*MN2*MR2*pq-2*f4A*f6A*MN2*MR2*pq+f3A*f4A*MR3*pq
       +f3V*f4V*MR3*pq-f3A*f6A*MR3*pq+2*f4A2*MN*MR3*pq-2*f4V2*MN*MR3*pq
       -2*f4V*f5V*MN*MR3*pq-2*f4A*f6A*MN*MR3*pq+2*f3A2*pq2+2*f3V2*pq2
       +2*f5A*f6A*pq2+f5V2*MN2*pq2+f6A2*MN2*pq2+2*f3A*f4A*MR*pq2+2*f3V*f4V*MR*pq2
       +2*f3V*f5V*MR*pq2-f5V2*MN*MR*pq2+f6A2*MN*MR*pq2+2*f4A2*MR2*pq2+2*f4V2*MR2*pq2
       +2*f4V*f5V*MR2*pq2-2*f4A*f6A*MR2*pq2+f5V2*pq3+f6A2*pq3-f3A2*MN2*Q2-f3V2*MN2*Q2
       -2*f5A*f6A*MN2*Q2-2*f5A*f6A*MN*MR*Q2-f3A*f4A*MN2*MR*Q2
       -f3V*f4V*MN2*MR*Q2-2*f3A*f4A*MN*MR2*Q2+2*f3V*f4V*MN*MR2*Q2
       -f4A2*MN2*MR2*Q2-f4V2*MN2*MR2*Q2+f6A2*MN2*MR2*Q2-f4A2*MN*MR3*Q2+f4V2*MN*MR3*Q2
       +f6A2*MN*MR3*Q2-f3A2*pq*Q2-f3V2*pq*Q2-2*f5A*f6A*pq*Q2-2*f6A2*MN2*pq*Q2
       -f3A*f4A*MR*pq*Q2-f3V*f4V*MR*pq*Q2-f3A*f6A*MR*pq*Q2
       -2*f6A2*MN*MR*pq*Q2-f4A2*MR2*pq*Q2-f4V2*MR2*pq*Q2+f6A2*MR2*pq*Q2
       -2*f6A2*pq2*Q2+f6A2*MN2*Q4+f6A2*MN*MR*Q4+f6A2*pq*Q4))/(3.*MR2);

  W5 = 3.*(2*f5A2*MN2
       +2*f5A2*MN*MR+f3A*f5A*MN2*MR+2*f3A*f5A*MN*MR2+2*f4A*f5A*MN2*MR2
       +f3A*f5A*MR3+2*f4A*f5A*MN*MR3+2*f5A2*pq+2*f3A2*MN2*pq+2*f3V2*MN2*pq
       +2*f5A*f6A*MN2*pq+2*f3A*f5A*MR*pq+2*f5A*f6A*MN*MR*pq
       +2*f3A*f4A*MN2*MR*pq+2*f3V*f4V*MN2*MR*pq+2*f3V*f5V*MN2*MR*pq
       +2*f3A2*MR2*pq+2*f3V2*MR2*pq+2*f4A*f5A*MR2*pq+4*f3A*f4A*MN*MR2*pq
       -4*f3V*f4V*MN*MR2*pq-4*f3V*f5V*MN*MR2*pq+2*f4A2*MN2*MR2*pq
       +2*f4V2*MN2*MR2*pq+4*f4V*f5V*MN2*MR2*pq+2*f5V2*MN2*MR2*pq+2*f3A*f4A*MR3*pq
       +2*f3V*f4V*MR3*pq+2*f3V*f5V*MR3*pq+2*f4A2*MN*MR3*pq-2*f4V2*MN*MR3*pq
       -4*f4V*f5V*MN*MR3*pq-2*f5V2*MN*MR3*pq+2*f3A2*pq2+2*f3V2*pq2+2*f5A*f6A*pq2
       +2*f3A*f4A*MR*pq2+2*f3V*f4V*MR*pq2+2*f3V*f5V*MR*pq2+2*f4A2*MR2*pq2
       +2*f4V2*MR2*pq2+4*f4V*f5V*MR2*pq2+2*f5V2*MR2*pq2-2*f5A*f6A*MN2*Q2+f3A*f5A*MR*Q2
       -2*f5A*f6A*MN*MR*Q2-f3A*f6A*MN2*MR*Q2-2*f3A*f6A*MN*MR2*Q2
       -2*f4A*f6A*MN2*MR2*Q2-f3A*f6A*MR3*Q2-2*f4A*f6A*MN*MR3*Q2
       -2*f5A*f6A*pq*Q2+2*f5V2*MN2*pq*Q2+2*f3V*f5V*MR*pq*Q2
       -2*f5V2*MN*MR*pq*Q2-2*f4A*f6A*MR2*pq*Q2+2*f5V2*pq2*Q2-f3A*f6A*MR*Q4)/(3.*MR2);

  double s1 =    W1 * (Q2+Mmu2)
               + W2 * (2*pk*pk-2*pq*pk+MN*qk)
               - W3 * (pq*qk+Q2*pk)
               + W4 * Mmu2*(Q2+Mmu2)/2.
               - W5 * 2*Mmu2*pk;

  double xsec = kGF_2/4./kPi*kCos8c_2/MN2/E2*W*MR*Gamma_R/kPi/Breit_Wigner*pauli*s1;

  return xsec;
}
//____________________________________________________________________________
double P33PaschosLalakulichPXSec::Pauli(double Q2, double W) const
{
// Pauli suppression for deuterium with Fermi momentum 0.160 GeV

  // ---remove--- this value from here & ask GENIE for D2 Fermi momentum
  double qF=0.160; /* 0.160 deuterium */

  double Paulii = 0;

  double p_pi_star = this->PPiStar(W);
  double nu_star   = this->NuStar(Q2,W);

  double p_pi_star_2 = TMath::Power(p_pi_star,   2);
  double p_pi_star_4 = TMath::Power(p_pi_star_2, 2);
  double nu_star_2   = TMath::Power(nu_star,     2);

  double q3  = TMath::Sqrt(Q2+nu_star_2);
  double q6  = TMath::Power(q3,2);
  double q12 = TMath::Power(q6,2);

  double qF2 = TMath::Power(qF,2);
  double qF3 = TMath::Power(qF,3);

  if ( q3+p_pi_star < 2*qF )
  {
     Paulii = ( (3*q6 + p_pi_star_2)/2/qF
               -(5*q12+p_pi_star_4 + 10*q6*p_pi_star_2)/40/qF3
              )/2/q3;
  }
  if ( (q3+p_pi_star > 2*qF) && (q3-p_pi_star < 2*qF) )
  {
     double tmp1 = TMath::Power( q3+p_pi_star, 2 );
     double tmp2 = TMath::Power( q3-p_pi_star, 3 );
     double tmp3 = TMath::Power( q3-p_pi_star, 5 );

     Paulii = (tmp1-4.0*qF2/5.0 - tmp2/2/qF + tmp3/40/qF3)/4/p_pi_star/q3;
  }
  if ( q3-p_pi_star > 2*qF )
  {
     Paulii = 1;
  }

  return Paulii;
}
//____________________________________________________________________________
double P33PaschosLalakulichPXSec::Nu(double Q2, double W) const
{
  return (TMath::Power(W,2) - kNucleonMass_2 + Q2)/2/kNucleonMass;
}
//____________________________________________________________________________
double P33PaschosLalakulichPXSec::PPiStar(double W) const
{
  double W2 = TMath::Power(W,2);
  double a  = TMath::Power(kNucleonMass+kPionMass,2);
  double b  = TMath::Power(kNucleonMass-kPionMass,2);

  return TMath::Sqrt( (W2-a)*(W2-b) )/2/W;
}
//____________________________________________________________________________
double P33PaschosLalakulichPXSec::NuStar(double Q2, double W) const
{
  return (TMath::Power(W,2) - kNucleonMass_2 - Q2)/2/W;
}
//____________________________________________________________________________
double P33PaschosLalakulichPXSec::MA(void) const
{
// Allow the default (const) Paschos-Lalakulich RES Ma (Axial-Mass) to be
// overriden from  a value at the algorithm's config registry

  if( fConfig->Exists("Ma") ) return fConfig->GetDouble("Ma");
  else                        return kPlResMa;
}
//____________________________________________________________________________
double P33PaschosLalakulichPXSec::MV(void) const
{
// Allow the default (const) Paschos-Lalakulich RES Mv (Vector-Mass) to be
// overriden from a value at the algorithm's config registry

  if( fConfig->Exists("Mv") ) return fConfig->GetDouble("Mv");
  else                        return kPlResMv;
}
//____________________________________________________________________________


