#include "VariablesPerformance.h"
void SetStyle();
void fitHistoPerformance()
{

  //gStyle->SetOptFit(0111);
  //gStyle->SetMarkerStyle(20);
  //gStyle->SetMarkerSize(0.5);
  //gStyle->SetMarkerColor(kBlack);
  //gStyle->SetLineColor(kBlack);

  // TList *listOfOutput = new TList();
  TFile *outVisual = new TFile("out_Visual.root");

  //-------------------------------------------------------------------------------------------------------------
  // Definisco le variabili per i fit

  // TF1 *fDelta0 = new TF1("fDelta0", "gaus", -2000, 2000);
  TF1 *fDelta = new TF1("fDelta", "gaus", -2000, 2000);
  Float_t RefSigmaDelta[nPRanges][nEtaRanges][nPhiRanges], RefSigmaErrDelta[nPRanges][nEtaRanges][nPhiRanges];
  Float_t MeanSigmaDelta[nPRanges][nEtaRanges][nPhiRanges], MeanSigmaErrDelta[nPRanges][nEtaRanges][nPhiRanges];
  Float_t RefSigmaDelta0[nPRanges], RefSigmaErrDelta0[nPRanges], MeanSigmaDelta0[nPRanges], MeanSigmaErrDelta0[nPRanges]; // in tutti gli angoli
  Float_t PMiddle[nPRanges];
  Float_t PhiMiddle[nPhiRanges];

  TH1F *h0;
  TH1D *h, *h2;
  TH3D *h3d;
  TH2D *h2d;

  //-------------------------------------------------------------------------------------------------------------
  // Fitto ed estraggo la sigma della reference

  h0 = (TH1F *)outVisual->Get("hReference");
  TF1 *fRefg = new TF1("fRefg", "gaus", -2000, 2000);
  h0->Fit(fRefg, "R,WW");

  RefSigma = fRefg->GetParameter(2) / sqrt(2);
  RefSigmaErr = fRefg->GetParError(2) / sqrt(2);

  //-------------------------------------------------------------------------------------------------------------
  // Estraggo tutti i parametri

  for (Int_t i = 0; i < nPRanges; i++)
  {
    iP = PRangeMin + i * DeltaPRange;
    PMiddle[i] = iP + DeltaPRange / 2;
    h0 = (TH1F *)outVisual->Get(Form("hDelta_%.1f_%.1f", iP, iP + DeltaPRange));
    h0->Fit(fDelta, "R,WW");
    RefSigmaDelta0[i] = sqrt(fDelta->GetParameter(2) * fDelta->GetParameter(2) - RefSigma * RefSigma);
    RefSigmaErrDelta0[i] = fDelta->GetParError(2) / fDelta->GetParameter(2) * RefSigmaDelta0[i];
    MeanSigmaDelta0[i] = fDelta->GetParameter(1);
    MeanSigmaErrDelta0[i] = fDelta->GetParError(1);
    h3d = (TH3D *)outVisual->Get(Form("hDelta3d_%.1f_%.1f", iP, iP + DeltaPRange));
    for (Int_t j = 0; j < nEtaRanges; j++)
    {
      jEta = EtaRangeMin + j * DeltaEtaRange;
      for (Int_t k = 0; k < nPhiRanges; k++)
      {
        if (i == 0 && j == 0)
          PhiMiddle[k] = kPhi + DeltaPhiRange / 2;
        kPhi = PhiRangeMin + k * DeltaPhiRange;
        h = h3d->ProjectionX("h", h3d->GetYaxis()->FindBin(jEta), h3d->GetYaxis()->FindBin(jEta + DeltaEtaRange), h3d->GetZaxis()->FindBin(kPhi), h3d->GetZaxis()->FindBin(kPhi + DeltaPhiRange));
        h->Fit(fDelta, "R,WW");
        //if (i==(nPRanges-5)&&j==(nEtaRanges-2)&&k==(nPhiRanges-2))
        //{TCanvas *c44 = new TCanvas("c44", "c44"); h->Draw();}
        RefSigmaDelta[i][j][k] = sqrt(fDelta->GetParameter(2) * fDelta->GetParameter(2) - RefSigma * RefSigma);
        RefSigmaErrDelta[i][j][k] = fDelta->GetParError(2) * RefSigmaDelta[i][j][k] / fDelta->GetParameter(2);
        MeanSigmaDelta[i][j][k] = fDelta->GetParameter(1);
        MeanSigmaErrDelta[i][j][k] = fDelta->GetParError(1);
      }
    }
  }

  //---------------------------------------------------------
  // Stampa delle risoluzioni

  for (Int_t i = 0; i < nPRanges; i++)
    cout << "\nP= (" << PRangeMin + i * DeltaPRange << " , " << PRangeMin + (i + 1) * DeltaPRange << ") --> Sigma= (" << RefSigmaDelta0[i] << " +- " << RefSigmaErrDelta0[i] << ")" << endl;
  cout << endl;

  ///////// DA CANCELLARE - Stampa di tutti i valori

  /*   for (Int_t i = 0; i < nPRanges; i++)
        for (Int_t j = 0; j < nEtaRanges; j++)
          for (Int_t k = 0; k < nPhiRanges; k++)
            cout << i << j << k << " " << RefSigmaDelta[i][j][k] << " " << RefSigmaErrDelta[i][j][k] << " " << MeanSigmaDelta[i][j][k] << " " << MeanSigmaErrDelta[i][j][k] << endl;
   */


/*   TCanvas *c3 = new TCanvas("c3", "c3");
  c3->cd();
  h2d = (TH2D *)outVisual->Get("ht_texp_FT0VSP");
  h2 = h2d->ProjectionY("h2", h2d->GetXaxis()->FindBin(1.3), h2d->GetXaxis()->FindBin(1.4));
  h2->SetName("ht_texp_FT0VSPint");
  h2->Fit(fDelta, "R,WW");
  h2->Draw();
  return; */
  outVisual->Close();

  //----------------------------------------------------------------------------------------
  // Output plots

  TFile *out_Plots = new TFile("out_Plots.root", "RECREATE");

  //-------------------------------------------------------------------------------------------------------------
  // Fitto una slice di p dell'istogramma ht_texp_FT0VSP

  // cout<<"\n\n"<<h2d->GetXaxis()->FindBin(1.3)<<"  "<<h3d->GetXaxis()->FindBin(1.4)<<endl;
  // return;

  // h->Write();
  // cout<<"\n\nSigma= "<<sqrt(fDelta->GetParameter(2) * fDelta->GetParameter(2) - RefSigma * RefSigma)<<" Err= "<<fDelta->GetParError(2) * RefSigmaDelta[i][j][k] / fDelta->GetParameter(2)<<endl;

  // MeanP
  TGraphErrors *MeanP = new TGraphErrors(nPRanges, PMiddle, MeanSigmaDelta0, 0, MeanSigmaErrDelta0);
  MeanP->SetTitle("Mean vs p interval");
  MeanP->SetName("MeanP");
  TAxis *XaxisMeanP = MeanP->GetXaxis();
  TAxis *YaxisMeanP = MeanP->GetYaxis();
  XaxisMeanP->SetTitle("p (GeV/c)");
  YaxisMeanP->SetTitle("Mean #Delta#Deltat (ps)");
  MeanP->Write();

  // SigmaP
  TGraphErrors *SigmaP = new TGraphErrors(nPRanges, PMiddle, RefSigmaDelta0, 0, RefSigmaErrDelta0);
  SigmaP->SetTitle("#sigma_{TOF} vs p interval");
  SigmaP->SetName("SigmaP");
  TAxis *XaxisSigmaP = SigmaP->GetXaxis();
  TAxis *YaxisSigmaP = SigmaP->GetYaxis();
  XaxisSigmaP->SetTitle("p (GeV/c)");
  YaxisSigmaP->SetTitle("#sigma_{TOF} #oplus #sigma_{Trk} (ps)");
  SigmaP->Write();

  // EtaPhi
  TMultiGraph *EtaPhiP[nPRanges], *EtaPhiPMean[nPRanges];
  TAxis *XaxisEtaPhiP[nPRanges], *XaxisEtaPhiPMean[nPRanges];
  TAxis *YaxisEtaPhiP[nPRanges], *YaxisEtaPhiPMean[nPRanges];

  TGraphErrors *graphEtaP[nEtaRanges], *graphEtaPMean[nEtaRanges];

  for (Int_t i = 0; i < nPRanges; i++)
  {
    iP = PRangeMin + i * DeltaPRange;

    EtaPhiP[i] = new TMultiGraph();
    EtaPhiP[i]->SetTitle(Form("#sigma_{TOF} shift for p (%.1f , %.1f)", iP, iP + DeltaPRange));
    XaxisEtaPhiP[i] = EtaPhiP[i]->GetXaxis();
    YaxisEtaPhiP[i] = EtaPhiP[i]->GetYaxis();
    EtaPhiP[i]->SetName(Form("EtaPhiP%i", i));
    XaxisEtaPhiP[i]->SetTitle("#varphi (rad)");
    YaxisEtaPhiP[i]->SetTitle("#sigma_{TOF} (ps)");

    EtaPhiPMean[i] = new TMultiGraph();
    EtaPhiPMean[i]->SetTitle(Form("Mean Value shift for p (%.1f , %.1f)", iP, iP + DeltaPRange));
    XaxisEtaPhiPMean[i] = EtaPhiPMean[i]->GetXaxis();
    YaxisEtaPhiPMean[i] = EtaPhiPMean[i]->GetYaxis();
    EtaPhiPMean[i]->SetName(Form("EtaPhiPMean%i", i));
    XaxisEtaPhiPMean[i]->SetTitle("#varphi (rad)");
    YaxisEtaPhiPMean[i]->SetTitle("Mean Value Shift (ps)");

    for (Int_t j = 0; j < nEtaRanges; j++)
    {
      jEta = EtaRangeMin + j * DeltaEtaRange;
      /*       if (i != 0)
            {
              delete graphEtaP[j];
              graphEtaP[j] = nullpr;
              delete graphEtaPMean[j];
              graphEtaPMean[j] = nullpr;
            } */
      graphEtaP[j] = new TGraphErrors(nPhiRanges, PhiMiddle, RefSigmaDelta[i][j], 0, RefSigmaErrDelta[i][j]);
      graphEtaP[j]->SetName(Form("#eta (%.1f , %.1f)", jEta, jEta + DeltaEtaRange));
      EtaPhiP[i]->Add(graphEtaP[j]);

      graphEtaPMean[j] = new TGraphErrors(nPhiRanges, PhiMiddle, MeanSigmaDelta[i][j], 0, MeanSigmaErrDelta[i][j]);
      graphEtaPMean[j]->SetName(Form("#eta (%.1f , %.1f)", jEta, jEta + DeltaEtaRange));
      EtaPhiPMean[i]->Add(graphEtaPMean[j]);
    }

    EtaPhiP[i]->Write();
    EtaPhiPMean[i]->Write();
  }
  out_Plots->Close();

  // return;
}
