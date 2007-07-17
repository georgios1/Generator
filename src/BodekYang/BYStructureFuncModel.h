//____________________________________________________________________________
/*!

\class    genie::BYStructureFuncModel

\brief    Abstract class. Provides common implementation for concrete
          DISStructureFuncModelI objects computing the Bodek Yang structure
          functions.

\ref      hep-ph/0411202

\author   Costas Andreopoulos <C.V.Andreopoulos@rl.ac.uk>
          STFC, Rutherford Appleton Laboratory

\created  September 28, 2004

*/
//____________________________________________________________________________

#ifndef _BODEK_YANG_STRUCTURE_FUNCTION_MODEL_H_
#define _BODEK_YANG_STRUCTURE_FUNCTION_MODEL_H_

#include "PartonModel/DISStructureFuncModel.h"
#include "Interaction/Interaction.h"
#include "PDF/PDFModelI.h"

namespace genie {

class BYStructureFuncModel : public DISStructureFuncModel {

public:
  virtual ~BYStructureFuncModel();

  //! Overload Algorithm::Configure() to read the config. registry
  //! at the algorithm initialization and set private data members
  void Configure (const Registry & config);
  void Configure (string param_set);

protected:

  void Init         (void);
  void ReadBYParams (void);

  //! protected constructors - abstract class
  BYStructureFuncModel(string name);
  BYStructureFuncModel(string name, string config);

  //! override part of the DISStructureFuncModel implementation
  //! to compute all the corrections applied by the Bodek-Yang model.
  double ScalingVar (const Interaction * i) const;
  void   KFactors   (const Interaction * i, double & kuv, 
                         double & kdv, double & kus, double & kds) const;

  //! Bodek-Yang model-specific parameters

  double fA;     ///< better scaling var parameter A
  double fB;     ///< better scaling var parameter B
  double fCsU;   ///< U-sea K factor parameter 
  double fCsD;   ///< D-sea K factor parameter 
  double fCv1U;  ///< U-val K factor parameter 
  double fCv2U;  ///< U-val K factor parameter 
  double fCv1D;  ///< D-val K factor parameter 
  double fCv2D;  ///< D-val K factor parameter 
};

}         // genie namespace

#endif    // _BODEK_YANG_STRUCTURE_FUNCTION_MODEL_H_
