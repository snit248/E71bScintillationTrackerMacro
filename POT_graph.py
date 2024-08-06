import ROOT
import matplotlib.pyplot as plt
import matplotlib.dates as mdates
from datetime import datetime, timedelta
import numpy as np

def POT_graph():
    # ROOTファイルを開く
    file = ROOT.TFile.Open("../../POT/E71b_2023_POT/20231123to1225BSD.root")
    if not file or file.IsZombie():
        print("Could not open the file.")
        return

    # TTreeを読み込む
    tree = file.Get("bsd")
    if not tree:
        print("Could not read the TTree.")
        file.Close()
        return

    # データを読み込み、全てのPOTを積算
    total_pot = 0
    daily_total_pot = {}
    nentries = tree.GetEntries()
    for i in range(nentries):
        tree.GetEntry(i)
        trg_sec = getattr(tree, "trg_sec")
        ct_np_array = np.array(getattr(tree, "ct_np"))

        # Unix時間を日付に変換
        unixTime = trg_sec[2] + 32400  # JSTに変換
        date = datetime.fromtimestamp(unixTime).date()

        # 積算POTを計算
        total_pot += ct_np_array[4][0]  # numpy 配列を使用してアクセス
        daily_total_pot[date] = total_pot
    # ファイルを閉じる
    file.Close()

    # グラフを作成
    dates = list(daily_total_pot.keys())
    pots = list(daily_total_pot.values())

    plt.figure(figsize=(10, 6))
    plt.plot(dates, pots, marker='o')

    # X軸を日付フォーマットに設定
    plt.gca().xaxis.set_major_formatter(mdates.DateFormatter("%m/%d"))
    plt.gca().xaxis.set_major_locator(mdates.DayLocator())
    plt.gcf().autofmt_xdate()

    plt.title("Accumulated POT")
    plt.xlabel("Date")
    plt.ylabel("Accumulated POT")
    plt.show()

POT_graph()
