#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <array>


class Importance
{
    public:
        // Basic constructors.
        Importance() = default; // default constructor.
        Importance(Importance const & ); // copy constructor.
        Importance(Importance       &&); // move constructor.
        Importance & operator=(Importance const & ); // copy assignment operator.
        Importance & operator=(Importance       &&); // move assignment operator.
        ~Importance() = default; // default destruct

        std::map<std::string, int> GetBetaParams(void) const
        {
            return beta_params_;
        }

        void PrintMap(std::map<std::string, int>& m);

        std::vector<double> PolyBiquadratic(std::vector<double> x_data, std::vector<double> beta);
        std::vector<double> InitPolyBiquadratic(std::vector<double> x_data, std::vector<double> y_data);
        std::vector<double> PseudoVoight(std::vector<double> x_data, std::vector<double> beta);
        std::array<double, 4> InitPseudoVoight(std::vector<double> x_data, std::vector<double> y_data);
        std::vector<double> GaussianAmplitude(std::vector<double> x_data, std::vector<double> beta);
        std::vector<double> GeneralizedLorentz(std::vector<double> x_data, std::vector<double> beta);
        std::vector<double> LorentzAmplitude(std::vector<double> x_data, std::vector<double> beta);
};
