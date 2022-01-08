#include <iostream>
#include <vector>
#include <cstdint>
#include <cmath>

class Poly1d
{
    private:
         std::vector<double> coefficients_{};
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
            
        std::vector<double> fit (std::vector<double> x_data, std::vector<double>, int order)
        {
           
        }        
};

//============================================================================
// Method Description:
//std::vector<double> Poly1d :: fit(const std::vector<double>& xValues, const std::vector<double>& yValues, uint8_t polyOrder)
//{
//  int numMeasurements = xValues.size();
//
//  NdArray<double> a(numMeasusrements, polyOrder + 1);
//  for (uint32 measIdx = 0; measIdx < numMeasurements; ++measIdx)
//  {
//      const auto xDouble = static_cast<double>(xValues[measIdx]);
//      for (uint8 order = 0; order < a.numCols(); ++order)
//      {
//          a(measIdx, order) = utils::power(xDouble, order);
//      }
//  }
//
//  NdArray<double> aInv;
//  if (a.issquare())
//  {
//      aInv = linalg::inv(a);
//  }
//  else
//  {
//      // psuedo-inverse
//      auto aT = a.transpose();
//      auto aTaInv = linalg::inv(aT.dot(a));
//      aInv = aTaInv.dot(aT);
//  }
//  
//  auto x = aInv.dot(yValues.template astype<double>());
//  return Poly1d<double>(x);
//}

