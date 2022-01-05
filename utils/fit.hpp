#ifndef FIT_HPP_INCLUDED
#define FIT_HPP_INCLUDED

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <array>


class Fit
{
    private:
        std::map<std::string, int> beta_params_ = {{"poly4", 4},
                                            {"pseudo_voight", 4},
                                            {"gauss_amp", 3},
                                            {"gen_lor_amp", 4},
                                            {"lor_amp", 3}};
    public:
        // Basic constructors.
        Fit() = default; // default constructor.
        Fit(Fit const & ); // copy constructor.
        Fit(Fit       &&); // move constructor.
        Fit & operator=(Fit const & ); // copy assignment operator.
        Fit & operator=(Fit       &&); // move assignment operator.
        ~Fit() = default; // default destruct

        std::map<std::string, int> GetBetaParams(void) const
        {
            return beta_params_;
        }

        void PrintMap(std::map<std::string, int>& m);

        std::vector<double> PolyBiquadratic(std::vector<double> x_data, std::vector<double> beta);
        std::vector<double> InitPolyBiquadratic(std::vector<double> x_data, std::vector<double> y_data);
        std::vector<double> PseudoVoight(std::vector<double> x_data, std::vector<double> beta);
        std::vector<double> InitPseudoVoight(std::vector<double> &x_data, std::vector<double> &y_data);
        std::vector<double> GaussianAmplitude(std::vector<double> x_data, std::vector<double> beta);
        std::vector<double> GeneralizedLorentz(std::vector<double> x_data, std::vector<double> beta);
        std::vector<double> LorentzAmplitude(std::vector<double> x_data, std::vector<double> beta);
};

#endif // FIT_HPP_INCLUDED
