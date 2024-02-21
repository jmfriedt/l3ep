8-bit microcontroller Fast Fourier Transform (FFT) example based on the Maxim 
Applicaiton note 3722 provided for reference since hardly found on the web any 
more. The number of intput samples is fixed (N=256 in maxqfft.h) and the software
computes the magnitude of the FFT of the real signal, hence providing N/2 output
items.

See J.-M Friedt, <a href="http://jmfriedt.free.fr/glmf_an3722.pdf">Du domaine temporel 
au domaine spectral dans 2,5 kB de mémoire : transformée de Fourier rapide sur Atmega32U4 
et quelques subtilités du C</a>, Hackable 49 (July/August 2023) [in French]
