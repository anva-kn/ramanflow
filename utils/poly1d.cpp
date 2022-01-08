#include <iostream>
#include <vector>
//#include <cstdint>
#include <cmath>
#include <Eigen/Dense>
#include <Eigen/QR>

class Poly1d
{
    protected:
         std::vector<double> coefficients_;
    public:
        //============================================================================
        // Method Description:
        Poly1d() = default;

        //============================================================================
        // Method Description:
        std::vector<double> getCoeffs()
        {
            return coefficients_;
        }

        void init (const std::vector<double>& inValues)
        {
            // std::vector<double> coefficients_;
            for (auto value : inValues)
            {
                coefficients_.push_back(value);
            }
        }
            
        std::vector<double> evaluate (std::vector<double> inValues)
        {
            std::vector<double> result;
            
            for (int i = 0; i < inValues.size(); i++)
            {
                double polyValue = 0.0;
                int power = inValues.size() - 1;
                for (auto coeff : coefficients_)
                {
                    polyValue += coeff * pow(inValues[i], power);
                    power--;
                }
                result.push_back(polyValue);
            }
            
            return result;
        }
            
        void fit (std::vector<double> x_data, std::vector<double> y_data, std::vector<double> &coeff, int order)
        {
            Eigen::MatrixXd W(x_data.size(), order + 1); 
            Eigen::MatrixXd I = Eigen::VectorXd::Map(&y_data.front(), y_data.size());
            Eigen::VectorXd result;

            assert(x_data.size() == y_data.size());
            assert(x_data.size() >= order + 1);

            for(std::size_t i = 0; i < x_data.size(); i++)
            {
                for (std::size_t j = 0; j < order + 1; j++)
                {
                    W(i, j) = std::pow(x_data.at(i), j);
                }
            }

//            std::cout<< W << std::endl;

            result = W.householderQr().solve(I);
            coeff.resize(order + 1);

            for(std::size_t k = 0; k < order + 1; k++)
            {
                coeff[k] = result[k];
            }
        }        
};

