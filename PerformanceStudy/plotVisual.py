#!/usr/bin/env python3

from ROOT import TFile, TCanvas, TChain, TH1, TH2, TColor, TObjArray, TF1
import matplotlib.pyplot as plt
import sys
import os
if 1:
    from plotting import draw_nice_canvas, update_all_canvases, draw_nice_frame, set_nice_frame, draw_nice_legend, draw_nice_frame, draw_label, draw_pave
sys.path.append(os.path.abspath("/Users/sofiastrazzi/Documents/TOFperformance/Commissioning/PerformanceStudy/"))

def main(filename="out_Visual.root"):
    f = TFile(filename, "READ")
    f.ls()
    hhReference = f.Get("hReference")
    draw_nice_frame(draw_nice_canvas("cReference"), hhReference, [0, hhReference.GetMaximum()*1.2], hhReference, "Counts")
    fgaus = hhReference.GetListOfFunctions().At(0)
    # fgaus = TF1("fgaus", "gaus", -500, 500)
    hhReference.Draw("SAME")
    # hhReference.Fit(fgaus, "QNRWW")
    # fgaus.Draw("same")
    fgaus.SetLineWidth(3)
    fgaus.SetLineColor(4)
    draw_label("Reference (p = 0.6 < p < 0.7 GeV/c)")
    draw_pave(["#mu = {:.3f}".format(fgaus.GetParameter(1)),
         "#sigma = {:.3f}".format(fgaus.GetParameter(2))])


    h2d = f.Get("ht_texp_FT0VSP")
    h1t_texp_FT0VSP = h2d.ProjectionY("h1t_texp_FT0VSP", 
        h2d.GetXaxis().FindBin(1.4), 
        h2d.GetXaxis().FindBin(1.5))
    draw_nice_frame(draw_nice_canvas("ct_texp_FT0VSP"), h1t_texp_FT0VSP, [0, h1t_texp_FT0VSP.GetMaximum()*1.2], "t-texpPi-t0 (ps)", "Counts")
    fgaus = hhReference.GetListOfFunctions().At(0)
    fgaus = TF1("fgaus", "gaus", -500, 500)
    h1t_texp_FT0VSP.Fit(fgaus, "QNRWW")
    h1t_texp_FT0VSP.Draw("SAME")
    fgaus.Draw("SAME")
    fgaus.SetLineWidth(3)
    fgaus.SetLineColor(2)
    #draw_label("{} [{:.2f},{:.2f}] {}".format("t-t\u209A-FT0", *binsx, histos[hn].GetXaxis().GetTitle()),0.95, 0.96, align=31)
    draw_label("t-texpPi-t0 (p = 1.4 < p < 1.5 GeV/c)")
    draw_pave(["#mu = {:.3f}".format(fgaus.GetParameter(1)),
         "#sigma = {:.3f}".format(fgaus.GetParameter(2))])
   
    update_all_canvases()
    input("Press enter to exit")


    fout = TFile("TOFPerf.root", "RECREATE")
    fout.cd()
    for i in [hhReference, h1t_texp_FT0VSP]:
        i.Write()

    f.Close()
    
if __name__ == "__main__":
    main()