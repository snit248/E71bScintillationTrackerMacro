import matplotlib.pyplot as plt

# Data for each module
#module_26 = {
#    "voltage": [57, 48.93, 39.04, 29.14, 19.23, 9.34],
#    "current": [7.32, 4.62, 3.53, 2.44, 1.42, 0.47]
#}
#module_25 = {
#    "voltage": [57, 48.64, 38.78, 28.92, 19.06, 9.21],
#    "current": [7.39, 4.40, 3.31, 2.22, 1.35, 0.55]
#}
#module_16 = {
#    "voltage": [57, 49.92, 39.94, 29.94, 19.95, 9.94],
#    "current": [8.77, 6.95, 4.99, 3.31, 1.71, 0.69]
#}
module_19 = {
    "voltage": [57, 50.07, 40.04, 30.01, 19.97, 9.95],
    "current": [7.61, 4.99, 3.75, 2.66, 1.57, 0.55]
}

#0119OCmeasurement
module_16 = {
    "voltage": [57.00, 49.86, 39.89, 29.89, 19.90, 9.91],
    "current": [14.23, 10.30, 7.03, 5.06, 2.51, 0.84]
}
module_18 = {
    "voltage": [57.00, 49.64, 39.67, 29.68, 19.70, 9.72],
    "current": [10.16, 8.04, 5.86, 4.11, 2.37, 0.91]
}
module_18dry = {
    "voltage": [56.61, 49.66, 39.68, 29.7, 19.72, 9.74],
    "current": [5.57, 4.77, 3.68, 2.51, 1.42, 0.55]
}
module_16_0130 = {
    "voltage": [57.00, 49.90, 39.94, 29.94, 19.94, 9.95],
    "current": [13.29, 9.65, 6.08, 4.04, 2.22, 0.69]
}

# Plotting the data
plt.figure(figsize=(10, 6))

plt.plot(module_16["voltage"], module_16["current"], label='Module 16(Actual channels:OC)', marker='o')
plt.plot(module_18["voltage"], module_18["current"], label='Module 18(Test:Wet converion board)', marker='o')
plt.plot(module_19["voltage"], module_19["current"], label='Module 19(Actual channels:Not OC)', marker='o')
plt.plot(module_18dry["voltage"], module_18dry["current"], label='Module 18(Test:Dry conversion board)', marker='o')
plt.plot(module_16_0130["voltage"], module_16_0130["current"], label='Module 16 1/31(Actual channels:OC)', marker='o')

plt.xlabel('Voltage (V)')
plt.ylabel('Current (µA)')
plt.title('Voltage vs Current for Four Modules')
plt.legend()
plt.grid(True)

plt.show()
