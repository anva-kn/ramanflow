# ramanflow
This is the repo started out as a NSD term project. 

It is a work in progress on high-perfromace data anlysis tools for spectroscopical data. 
That includes Raman, SERS, and HPLC.

As a proof of concept, first this will be implemented in Python, and then in C++.

The goal is to have a simple and easy to use interface, while maintaining high performance.
The target audience is researchers who are not familiar with programming, but need to do data analysis.


What it should have:
- [ ] Data loading
  - [ ] Load data from .txt where 1st column is x-axis, 2nd column is y-axis
  - [ ] Load data from .csv where 1st column is x-axis, and the rest are y-axis
  - [ ] Load data from .xlsx where 1st column is x-axis, and the rest are y-axis
- [ ] Data preprocessing
  - [ ] Baseline correction
    - [ ] Polynomial fitting
    - [ ] Asymmetric least square
    - [ ] Iterative smoothing
    - [ ] Iterative polynomial fitting
    - [ ] Spline fitting
    - [ ] Wavelet transform
    - [ ] Wavelet transform with adaptive threshold
  - [ ] Smoothing
    - [ ] Savitzky-Golay
    - [ ] Moving average
    - [ ] Gaussian filter
    - [ ] Median filter
    - [ ] Fourier filter
  - [ ] Normalization
    - [ ] Min-max normalization
    - [ ] Mean normalization
    - [ ] Normalization by value at a specific point
  - [ ] Peak detection
  - [ ] Peak fitting
    - [ ] Gaussian
    - [ ] Lorentzian
    - [ ] Voigt
    - [ ] Pseudo-Voigt
    - [ ] Pearson VII
    - [ ] Split Pearson VII
  - [ ] Peak deconvolution 
- [ ] Data visualization
    - [ ] Plotting
    - [ ] 3D plotting
      - [ ] Spectral map
      - [ ] Spectral image
      - [ ] Spectrogram
      - [ ] Waterfall
- [ ] Data analysis
  - [ ] PCA
  - [ ] NMF
  - [ ] Clustering
  - [ ] Classification
  - [ ] Regression
  - [ ] GAN For peak removal and background reconstruction
