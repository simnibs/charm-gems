#ifndef GEMS_PYKVLAFFINEREGISTRATION_H
#define GEMS_PYKVLAFFINEREGISTRATION_H

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
namespace py = pybind11;

#include "itkImage.h"
#include "kvlRegisterImages.h"
#include "itkAffineTransform.h"
#include "itkANTSNeighborhoodCorrelationImageToImageMetricv4.h"

class KvlAffineRegistration {
    typedef itk::Image< double, 3 > ImageType;
    typedef itk::AffineTransform< double, 3 >  AffineTransformType;
    typedef itk::ANTSNeighborhoodCorrelationImageToImageMetricv4<ImageType, ImageType> MetricType;
    kvl::RegisterImages<AffineTransformType, MetricType>::Pointer registerer;
    public:
        // Python accessible
        KvlAffineRegistration()
        {
            registerer = kvl::RegisterImages<AffineTransformType, MetricType>::New();
            //Set MI histogram bins
            registerer->SetCenterOfMassInit(true);
            std::vector<double> scales;
            scales.push_back(3.0);
            scales.push_back(2.0);
            MetricType::RadiusType neighborhoodRadius;
            neighborhoodRadius.Fill(5);
            registerer->GetMetric()->SetRadius(neighborhoodRadius);
            registerer->SetShrinkScales(scales);

        }
    
        KvlAffineRegistration(double transScale, int numIter,
        int radius, py::array_t<double> shrinkScales, double bgLevel,
        double sigma, bool useCenterOfMass, double sampRate,
        std::string interpolator)
        {
        
            py::buffer_info scales_info  = shrinkScales.request();
            const int numScales = scales_info.shape[0];
            std::vector<double> scales;
            
            for ( int scaleNum = 0; scaleNum < numScales; ++scaleNum ) {
            	 scales.push_back( shrinkScales.at(scaleNum) );
            }
            
            MetricType::RadiusType neighborhoodRadius;
            neighborhoodRadius.Fill(radius);
            registerer = kvl::RegisterImages<AffineTransformType, MetricType>::New();
            registerer->SetTranslationScale(transScale);
            registerer->SetNumberOfIterations(numIter);
            registerer->GetMetric()->SetRadius(neighborhoodRadius);
            registerer->SetShrinkScales(scales);
            registerer->SetBackgroundGrayLevel(bgLevel);
            registerer->SetSmoothingSigmas(sigma);
            registerer->SetCenterOfMassInit(useCenterOfMass);
            registerer->SetSamplingPercentage(sampRate);
            registerer->SetInterpolator(interpolator);
        }

        void ReadImages(const std::string &fileNameT1, const std::string &fileNameT2)
        {
            registerer->ReadImages(fileNameT1.c_str(), fileNameT2.c_str());
        }

        void InitializeTransform()
        {
            registerer->InitializeTransform();
        }

        void Register()
        {
            registerer->RunRegistration();
        }

        void WriteOutResults(std::string outImageFile)
        {
            registerer->WriteOutResults(outImageFile);
        }

        py::array_t<double> GetFinalTransformation();
};
#endif //GEMS_PYKVLAFFINEREGISTRATION_H
