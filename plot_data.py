import numpy as np
import datetime as dt
import matplotlib.pyplot as plt
import utils.my_plot_lib as mplt
import uncertainties as unc
from uncertainties import unumpy as unp

figs = [] # Do not touch this.

# Read data ------------------------
data = np.genfromtxt('DATALOG.TXT', skip_header=1)
time = [None]*len(data)
temperatures = [None]*len(data)
for k in range(len(data)):
	time[k] = dt.datetime(year=int(data[k][0]), month=int(data[k][1]), day=int(data[k][2]), hour=int(data[k][3]), minute=int(data[k][4]), second=int(data[k][5]))
	temperatures[k] = data[k][6:].tolist()
temperatures = list(map(list, zip(*temperatures))) # Transpose list.
for k in range(len(temperatures)):
	temperatures[k] = unp.uarray(temperatures[k], np.zeros(len(temperatures[k])) + 0.5) # This is the uncertainty in the datasheet https://datasheets.maximintegrated.com/en/ds/DS18B20.pdf
# Plot data -----------------------
f, axes = plt.subplots(1, sharex=True, figsize=(mplt.fig_width*mplt.fig_ratio[0]/25.4e-3, mplt.fig_width*mplt.fig_ratio[1]/25.4e-3)) # Create the figure for plotting.
figs.append(f)
f.subplots_adjust(hspace=0.3) # Fine-tune figure; make subplots close to each other and hide x ticks for all but bottom plot.
f.suptitle('Temperaturas en la galería')
for k in range(len(temperatures)):
	mplt.plot_yerr_filled(time, temperatures[k], axes, color=mplt.colors[k], label=r'$T_{:d}$'.format(k+1))
mplt.beauty_grid(axes)
axes.legend()
axes.set_xlabel('Fecha')
axes.set_ylabel('Temperatura (°C)')
# Save and show plot --------------
for k in range(len(figs)):
	figs[k].savefig(str(k+1) + '.' + mplt.image_format, bbox_inches='tight', dpi=mplt.dpi_rasterization)
mng = plt.get_current_fig_manager()
mng.resize(*mng.window.maxsize())
plt.show()
