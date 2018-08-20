import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import numpy as np
import uncertainties

dpi_rasterization = 200 # Resolution for png format.
image_format = 'png'
colors = [(255, 102, 0), (51, 153, 255), (51, 102, 0), (255, 0, 255)]
colors = np.array(colors)/255
fig_width = 250e-3 # Figure width in meters (size of the chart+axis_ticks, not counting legend, title, etc.).
fig_ratio = [1, 0.6] # XY ratio aspect of figures.

def plot_yerr_filled(x, y=[None], axes=plt, color=(0,0,0), alpha=0.25, linestyle='-', *args, **kwargs):
	"""Creates a 'filled error bar' plot. All keyword arguments in matplotlib functions are accepted.
	
	Parameters
	----------
	x : Array like (list of float, int, etc. or numpy.array or list of magnitude) of exact 'x' values (i.e. with no error, if 'x' is a list of 'magnitude' type objects, errors are discarded).
	y : Array like (list) of 'magnitude' type elements containing the 'y' values and the 'yerr' associated error.
	axis: matplotlib.axes (optional) in which to plot the data.
	color: tuple (r,g,b) defining the color.
	alpha: float between 0 and 1 indicating the transparency of the filled error bar.
	"""
	y_val = [None]*len(y)
	y_err = [None]*len(y)
	if y[0] != None:
		for i in range(len(x)): # Validation of x
			if isinstance(x[i], uncertainties.UFloat):
				x[i] = x[i].n
		for i in range(len(y)): # Validation of y
			if isinstance(y[i], uncertainties.UFloat):
				y_val[i] = y[i].n
				y_err[i] = y[i].s
			else:
				y_val[i] = y[i]
				y_err[i] = 0
	else:
		y = x;
		x = range(len(y))
		if isinstance(y[0], uncertainties.UFloat):
			y_val = [y[i].n for i in range(len(y))]
			y_err = [y[i].s for i in range(len(y))]
		else:
			y_val = y
			y_err = np.zeros(len(y))
	axes.fill_between(np.array(x), np.array(y_val)-np.array(y_err), np.array(y_val) + np.array(y_err), alpha=alpha, edgecolor=color, facecolor=color, linestyle=linestyle)
	lines = axes.plot(np.array(x), np.array(y_val), color=color, linestyle=linestyle, *args, **kwargs)
	return lines

def beauty_grid(ax):
	ax.grid(b=True, which='major', color='#000000', alpha=0.3, linestyle='-', linewidth=0.5)
	ax.grid(b=True, which='minor', color='#000000', alpha=0.15, linestyle='-', linewidth=0.25)
	ax.minorticks_on() # Enables minor ticks without text, only the ticks.

def scatter_zerr(x, y, z, axes=plt, color=(0,0,0), alpha=0.25, *args, **kwargs):
	if not isinstance(z[0], uncertainties.UFloat):
		raise ValueError('z must be an array of \'ufloat\'')
	zvals = [None]*len(z)
	for k in range(len(z)):
		zvals[k] = z[k].n
		axes.plot([x[k], x[k]], [y[k], y[k]], [zvals[k]+z[k].s, zvals[k]-z[k].s], marker='_', color=color, alpha=alpha, *args, **kwargs)
	axes.plot(x, y, zvals, color=color, linestyle='none', marker='.', *args, **kwargs)
