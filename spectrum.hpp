#include <vector>
#include <iostream>
#include <array>

class Spectrum {
    public:
        void setWavenumber(std::vector<double> wavenumber) {
            spectrumWavenumber = wavenumber;
        }
        std::vector<double> getWavenuber(){
            
        }
        void setIntensity(std::vector<double> intensity) {
            spectrumIntensity = intensity;
        }
        std::vector<double> getIntensity(){

        }

    private:
        int shape, size;
        char *dtype;
        std::vector<double> spectrumWavenumber;
        std::vector<double> spectrumIntensity;

}
