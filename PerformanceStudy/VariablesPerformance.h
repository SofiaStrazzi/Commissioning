#ifndef VARIABLESPERFORMANCE_H
#define VARIABLESPERFORMANCE_H

#include "TChain.h"
#include "TFile.h"
#include "TH1F.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TH3.h"
#include "TF1.h"
#include "TCut.h"
#include "TGraphErrors.h"
#include "TStyle.h"
#include "TLeaf.h"
#include "TMultiGraph.h"
#include "THnSparse.h"
#include "TMath.h"
#include "TList.h"
#include <iostream>
#include <fstream>

using namespace std;


//----------------------------------------------------------------------------------------
// RANGE DI IMPULSO, ETA E PHI valori da modificare

const Float_t PtRangeMin = 0.3, PtRangeMax = 1.3, DeltaPtRange = 0.1;
const Float_t EtaRangeMin = -0.8, EtaRangeMax = 0.8, DeltaEtaRange = 0.2;
const Float_t PhiRangeMin = 0, PhiRangeMax = TMath::Pi() * 2;
const Float_t DeltaPhiRange = TMath::Pi() * 2 / 18; // 0 | 2pi | pi/9

//----------------------------------------------------------------------------------------

const Int_t nPtRanges = (PtRangeMax - PtRangeMin) / DeltaPtRange;
const Int_t nEtaRanges = (EtaRangeMax - EtaRangeMin) / DeltaEtaRange;
const Int_t nPhiRanges = (PhiRangeMax - PhiRangeMin) / DeltaPhiRange;

Float_t RefSigma;
Float_t RefSigmaErr;
Float_t iPt, jEta, kPhi;


#endif