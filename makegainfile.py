import numpy as np
import pandas as pd

# Seed for reproducibility
np.random.seed(0)

# Generate data
index = np.arange(250)
pedestals = 800 + (np.random.rand(250) - 0.5) * 40  # 780 to 820 range
gains = 30 + (np.random.rand(250) - 0.5) * 2  # 29 to 31 range

# Create a DataFrame
data = np.vstack((index, pedestals, gains)).T
df = pd.DataFrame(data, columns=['index', 'pedestal', 'gain'])

# Convert to strings with specified formatting
df['index'] = df['index'].astype(int).astype(str)
df['pedestal'] = df['pedestal'].map('{:.2f}'.format)
df['gain'] = df['gain'].map('{:.4f}'.format)

# Save to a text file
filename = '../src/gain_data.dat'
df.to_csv(filename, sep=' ', index=False, header=False)
filename