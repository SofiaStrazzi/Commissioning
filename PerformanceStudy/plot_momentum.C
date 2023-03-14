#include "VariablesPerformance.h"

//----------------------------------------------------------------------------------------
// Chain

void addDFToChain(TString fileName, TChain &chain)
{
  TFile f(fileName, "READ");
  auto *lk = f.GetListOfKeys();
  for (int i = 0; i < lk->GetEntries(); i++)
  {
    TString dfname = lk->At(i)->GetName();
    if (!dfname.Contains("DF_"))
      continue;
    TString tname = Form("%s?#%s/O2deltatof", fileName.Data(), dfname.Data());
    cout << tname << endl;
    chain.Add(tname);
    // break;
  }
}

//----------------------------------------------------------------------------------------
// Inizio funzione principale

TList *plot_momentum(TString fileName = "${HOME}/cernbox2/LHC22m_523308_apass3_relval_cpu2/0/AnalysisResults_trees_TOFCALIB.root")
{
  TList *listOfOutput = new TList();
  gStyle->SetOptFit(0111);
  gStyle->SetMarkerStyle(20);
  gStyle->SetMarkerSize(0.5);
  gStyle->SetMarkerColor(kAzure);
  gStyle->SetLineColor(kAzure);

  TChain chain;
  if (fileName.EndsWith(".root"))
  {
    addDFToChain(fileName, chain);
  }
  else if (fileName.EndsWith(".txt"))
  {
    std::ifstream infile(fileName.Data());
    std::string line;
    while (std::getline(infile, line))
    {
      addDFToChain(line, chain);
    }
  }
  chain.Print();

  //----------------------------------------------------------------------------------------
  // Istogramma DoubleDelta per la referenza in una regione cinematica

  TFile *foutPt = new TFile("out_Visual.root", "RECREATE");

  cout << chain.GetEntries() << endl;
  chain.ls();
  TCanvas *c1 = new TCanvas("c1", "c1");
  c1->cd();
  // chain.Draw("fP");
  // TH2F *hsqrt = new TH2F("hsqrt", "hsqrt", 40, -10, 10, 100, -1000, 1000);
  // chain.Draw("fDoubleDelta:fPt>>hsqrt", "", "Colz");

  TH1F *hReference = new TH1F("hReference", "hReference", 100, -2000, 2000);
  TCut minP = "fP>0.6";
  TCut maxP = "fP<0.7";
  TCut minTOFChi2 = "fTOFChi2<5";
  TCut maxTOFChi2 = "fTOFChi2>=0";

  chain.Draw("fDoubleDelta>>hReference", minP && maxP && minTOFChi2 && maxTOFChi2, "Colz");
  TAxis *XhRef = hReference->GetXaxis();
  XhRef->SetTitle("#Delta#Delta_{T} (ps)");
  TF1 *fRefg = new TF1("fRefg", "gaus");
  fRefg->SetRange(-2000, 2000);
  hReference->Fit(fRefg, "R,WW");
  hReference->Draw();
  hReference->Write();

  RefSigma = fRefg->GetParameter(2) / sqrt(2);
  RefSigmaErr = fRefg->GetParError(2) / sqrt(2);

  cout << "Sigma Reference= (" << RefSigma << "+-" << RefSigmaErr << ")" << endl;

  //----------------------------------------------------------------------------------------
  // DoubleDelta per una diversa regione cinematica e per i diversi angoli

  Float_t Pt;
  chain.SetBranchAddress("fPt", &Pt);
  Float_t Eta;
  chain.SetBranchAddress("fEta", &Eta);
  Float_t Phi;
  chain.SetBranchAddress("fPhi", &Phi);
  Float_t TOFChi2;
  chain.SetBranchAddress("fTOFChi2", &TOFChi2);
  Float_t DoubleDelta;
  chain.SetBranchAddress("fDoubleDelta", &DoubleDelta);

  //---------------------------------------------------------
  // Definisco tutti gli istogrammi

  TCanvas *c2 = new TCanvas("c2", "c2");
  TH1F *hDelta0[nPtRanges];
  TAxis *XhDelta0[nPtRanges];
  TF1 *fDelta00[nPtRanges];
  TH3F *hDelta3d[nPtRanges];

  for (Int_t i = 0; i < nPtRanges; i++)
  {
    iPt = PtRangeMin + i * DeltaPtRange;
    hDelta0[i] = new TH1F(Form("hDelta_%.1f_%.1f", iPt, iPt + DeltaPtRange), Form("hDelta (%.1f , %.1f)", iPt, iPt + DeltaPtRange), 100, -2000, 2000);
    fDelta00[i] = new TF1(Form("fDelta_%.1f_%.1f", iPt, iPt + DeltaPtRange), "gaus", -2000, 2000);
    XhDelta0[i] = hDelta0[i]->GetXaxis();
    XhDelta0[i]->SetTitle("#Delta#Delta_{T} (ps)");
    // Plot in 3D con sigma eta phi
    hDelta3d[i] = new TH3F(Form("hDelta3d_%.1f_%.1f", iPt, iPt + DeltaPtRange), Form("hDelta3d (%.1f , %.1f);x;y;z", iPt, iPt + DeltaPtRange), 100, -2000, 2000, 100, EtaRangeMin, EtaRangeMax, 100, PhiRangeMin, PhiRangeMax);
  }

  //---------------------------------------------------------
  // Riempio tutti gli istogrammi

  const Int_t nentries = chain.GetEntries();
  for (Int_t ientry = 0; ientry < nentries; ientry++)
  {
    if (ientry % 10000 == 0)
      Printf("%.3f", float(ientry) / nentries * 100);
    chain.GetEntry(ientry);
    for (Int_t i = 0; i < nPtRanges; i++)
    {
      iPt = PtRangeMin + i * DeltaPtRange;
      if (Pt > iPt && Pt < (iPt + DeltaPtRange) && TOFChi2 < 5 && TOFChi2 >= 0)
      {
        hDelta0[i]->Fill(DoubleDelta);
        hDelta3d[i]->Fill(DoubleDelta, Eta, Phi);
        //cout << " " << DoubleDelta << " " << Eta << " " << Phi << endl;
      }
    }
  }

  // hDelta3d[8]->Draw(); return 0;

  //---------------------------------------------------------
  // Inserisco gli istogrammi in funzione del pT nel file visul_out (già fittati) insieme alla referenza

  for (Int_t i = 0; i < nPtRanges; i++)
  {
    hDelta0[i]->Fit(fDelta00[i], "R,WW");
    hDelta0[i]->Write();
  }

  //---------------------------------------------------------
  // Inserisco quelli vs pT Eta e Phi in histograms_out

  for (Int_t i = 0; i < nPtRanges; i++)
    hDelta3d[i]->Write();

  foutPt->Close();

  /* for (Int_t i = 0; i < nPtRanges; i++)
      for (Int_t j = 0; j < nEtaRanges; j++)
        for (Int_t k = 0; k < nPhiRanges; k++)
        {
          delete hDelta[i][j][k];
          hDelta[i][j][k] = nullptr;
        }
  */

  return listOfOutput;
}
