import uproot
import numpy as np
import matplotlib.pyplot as plt

# Define the paths to your ROOT files
sandmuon_file_path = "../E71b_ALL_ST/E71b_ST_data.root"
pot_file_path = "../../POT/E71b_ALL_POT/E71bALLPOT.root"

# Open the ROOT files using uproot
sandmuon_file = uproot.open(sandmuon_file_path)
pot_file = uproot.open(pot_file_path)

# Retrieve the trees from the ROOT files
sandmuon_tree = sandmuon_file["tree"]
pot_tree = pot_file["bsd"]

# Define the branches to read from the trees
sandmuon_branches = ["ADC", "PE", "UNIXTIME"]
pot_branches = ["trg_sec", "ct_np", "good_spill_flag"]

# Read the branches from the trees
sandmuon_data = sandmuon_tree.arrays(sandmuon_branches, library="np")
pot_data = pot_tree.arrays(pot_branches, library="np")

# Process POT data
total_pot = 0
daily_total_pot = []

for i in range(len(pot_data["trg_sec"])):
    unix_time = pot_data["trg_sec"][i][2] + 32400
    total_pot += pot_data["ct_np"][i][4][0]
    daily_total_pot.append((unix_time, total_pot))

# Process SandMuon data
total_muon = 0
daily_total_muon = []

for i in range(len(sandmuon_data["ADC"])):
    countX = 0
    countY = 0
    for ich in range(250):
        if sandmuon_data["PE"][i][ich] > 2.5 and ich not in [50, 115]:
            if ich < 125 or ich == 249:
                countY += 1
            else:
                countX += 1

    if countX > 1 and countY > 1:
        unix_time_mu = sandmuon_data["UNIXTIME"][i] + 32400
        total_muon += 1
        daily_total_muon.append((unix_time_mu, total_muon))

# Convert lists to numpy arrays for plotting
days_pot, pot_counts = zip(*daily_total_pot)
days_muon, muon_counts = zip(*daily_total_muon)

# Plot the data
fig, ax1 = plt.subplots()

ax1.set_xlabel('Day')
ax1.set_ylabel('POT', color='tab:blue')
ax1.plot(days_pot, pot_counts, color='tab:blue', label='POT')

ax2 = ax1.twinx()
ax2.set_ylabel('SandMuon Count', color='tab:red')
ax2.plot(days_muon, muon_counts, 'r+', label='SandMuon Count')

fig.tight_layout()
plt.show()