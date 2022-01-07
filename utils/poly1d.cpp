// This code was borrowed from NumCPP project. at https://github.com/dpilger26/NumCpp
#include <iostream>
#include <vector>
#include <cstdint>

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
        Poly1d(const std::vector<double>& inValues)
        {
            // std::vector<double> coefficients_;
            for (auto value : inValues)
            {
                coefficients_.push_back(value);
            }
        }
};

//============================================================================
// Method Description:
std::vector<double> Poly1d :: fit(const std::vector<double>& xValues, const std::vector<double>& yValues, uint8_t polyOrder)
{
    int numMeasurements = xValues.size();

    NdArray<double> a(numMeasusrements, polyOrder + 1);
    for (uint32 measIdx = 0; measIdx < numMeasurements; ++measIdx)
    {
        const auto xDouble = static_cast<double>(xValues[measIdx]);
        for (uint8 order = 0; order < a.numCols(); ++order)
        {
            a(measIdx, order) = utils::power(xDouble, order);
        }
    }

    NdArray<double> aInv;
    if (a.issquare())
    {
        aInv = linalg::inv(a);
    }
    else
    {
        // psuedo-inverse
        auto aT = a.transpose();
        auto aTaInv = linalg::inv(aT.dot(a));
        aInv = aTaInv.dot(aT);
    }
    
    auto x = aInv.dot(yValues.template astype<double>());
    return Poly1d<double>(x);
}

//============================================================================
// Method Description:
std::vector<double> Poly1d :: operator()(std::vector<double> inValue)
{
    int polyValue = 0;
    uint8_t power = 0;
    for (auto& coefficient : (*this)coefficients_)
    {
        polyValue += coefficient * utils::power(inValue, power++);
    }

    return polyValue;
}
