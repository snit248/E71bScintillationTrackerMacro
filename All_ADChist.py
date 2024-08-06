import ROOT

# ROOTファイルを開く
file = ROOT.TFile("../merge/All_1700986953_1701075597_conv.root", "READ")

# ツリーを取得する
tree = file.Get("tree")

# ヒストグラムを生成するための準備
histograms = {}
for i in range(250):  # 250チャンネルがあると仮定
    histograms[i] = ROOT.TH1F(f"hist_channel_{i}", f"ADC Histogram for Channel {i}", 100, 0, 4096) # ビン数、範囲は例です

# ツリーをループし、各エントリーのADC値をヒストグラムに填める
for entry in tree:
    for i in range(250):  # 250チャンネルがあると仮定
        adc_value = getattr(entry, f"adc[{i}]")
        histograms[i].Fill(adc_value)

# キャンバスにヒストグラムを描画する
canvas = ROOT.TCanvas()
for i in range(250):  # 250チャンネルがあると仮定
    histograms[i].Draw()
    canvas.Update()

# 必要に応じてヒストグラムを保存する
    histograms[i].SaveAs(f"histogram_channel_{i}.png")

# ファイルを閉じる
file.Close()
