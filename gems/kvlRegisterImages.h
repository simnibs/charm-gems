#ifndef __kvlRegisterImages_h
#define __kvlRegisterImages_h

#include "itkImageRegistrationMethodv4.h"
#include "itkImage.h"
#include "itkVersorRigid3DTransform.h"
#include "itkRegularStepGradientDescentOptimizerv4.h"
#include "itkCommand.h"
#include "itkMattesMutualInformationImageToImageMetricv4.h"

namespace kvl
{

class CommandIterationUpdate : public itk::Command
{
public:
  typedef CommandIterationUpdate Self;
  typedef itk::Command Superclass;
  typedef itk::SmartPointer<Self> Pointer;
  itkNewMacro(Self);

protected:
  CommandIterationUpdate(){};

public:
  typedef itk::RegularStepGradientDescentOptimizerv4<double> OptimizerType;
  typedef const OptimizerType * OptimizerPointer;

  void
  Execute(itk::Object *caller, const itk::EventObject &event) override
  {
    Execute((const itk::Object *)caller, event);
  }

  void
  Execute(const itk::Object *object, const itk::EventObject &event) override
  {
    auto optimizer = static_cast<OptimizerPointer>(object);
    if (!(itk::IterationEvent().CheckEvent(&event)))
    {
      return;
    }
    std::cout << optimizer->GetCurrentIteration() << "   ";
    std::cout << optimizer->GetValue() << "   ";
    std::cout << optimizer->GetCurrentPosition() << "   ";
    std::cout << m_CumulativeIterationIndex++ << std::endl;
  }

private:
  unsigned int m_CumulativeIterationIndex{0};
};

template <typename TRegistration>
class RegistrationInterfaceCommand : public itk::Command
{

  // Define \code{Self}, \code{Superclass}, \code{Pointer},
  // \code{New()} and a constructor as similar to the
  // class CommandIterationUpdate for
  // monitoring the evolution of the optimization process

public:
  typedef RegistrationInterfaceCommand Self;
  typedef itk::Command Superclass;
  typedef itk::SmartPointer<Self> Pointer;
  itkNewMacro(Self);

protected:
  RegistrationInterfaceCommand() = default;

  // Declare types for converting pointers
  // in the \code{Execute()} method.
public:
  typedef TRegistration RegistrationType;
  typedef RegistrationType * RegistrationPointer;
  typedef itk::RegularStepGradientDescentOptimizerv4<double> OptimizerType;
  typedef OptimizerType * OptimizerPointer;

  // Two arguments are passed to the \code{Execute()} method: the first
  // is the pointer to the object which invoked the event and the
  // second is the event that was invoked.
  void
  Execute(itk::Object *object, const itk::EventObject &event) override
  {

    // First we verify that the event invoked is of the right type,
    // \code{itk::MultiResolutionIterationEvent()}.
    // If not, we return without any further action.
    if (!(itk::MultiResolutionIterationEvent().CheckEvent(&event)))
    {
      return;
    }

    // We then convert the input object pointer to a RegistrationPointer.
    // Note that no error checking is done here to verify the
    // \code{dynamic\_cast} was successful since we know the actual object
    // is a registration method. Then we ask for the optimizer object
    // from the registration method.
    auto registration = static_cast<RegistrationPointer>(object);
    auto optimizer =
        static_cast<OptimizerPointer>(registration->GetModifiableOptimizer());

    unsigned int currentLevel = registration->GetCurrentLevel();
    typename RegistrationType::ShrinkFactorsPerDimensionContainerType shrinkFactors =
        registration->GetShrinkFactorsPerDimension(currentLevel);

    // // Check the values of the shrinking factors. If any of them is larger than the size of the fixed image, update to the size of the fixed image
    // auto sizeOfFixedImage=registration->GetFixedImage()->GetLargestPossibleRegion().GetSize();
    // Note: I tried to add the 2-D option in the program, but the class itk::ImageRegistrationMethodv4 implicitly calls the filter RecursiveGaussianImageFilter
    // This filter requires a minimum of 2(numberOfLevels -1) pixels along the dimension to be processed.
    // for (auto i=0; i < shrinkFactors.Length; ++i)
    // {
    //   shrinkFactors[i] = ((shrinkFactors[i] < sizeOfFixedImage[i]) ? shrinkFactors[i] : sizeOfFixedImage[i]);
    // }
    // registration->SetShrinkFactorsPerDimension(currentLevel, shrinkFactors);
    //
    // std::cout << registration->GetShrinkFactorsPerDimension(currentLevel) << std::endl;

    typename RegistrationType::SmoothingSigmasArrayType smoothingSigmas =
        registration->GetSmoothingSigmasPerLevel();

    std::cout << "-------------------------------------" << std::endl;
    std::cout << " Current level = " << currentLevel << std::endl;
    std::cout << "    Shrink factor = " << shrinkFactors << std::endl;
    std::cout << "    Smoothing sigma = ";
    std::cout << smoothingSigmas[currentLevel] << std::endl;
    std::cout << std::endl;

    // Large values of the learning rate will make the optimizer
    // unstable. Small values, on the other hand, may result in the optimizer
    // needing too many iterations in order to walk to the extrema of the cost
    // function. The easy way of fine tuning this parameter is to start with
    // small values, probably in the range of $\{1.0,5.0\}$. Once the other
    // registration parameters have been tuned for producing convergence, you
    // may want to revisit the learning rate and start increasing its value until
    // you observe that the optimization becomes unstable.  The ideal value for
    // this parameter is the one that results in a minimum number of iterations
    // while still keeping a stable path on the parametric space of the
    // optimization. Keep in mind that this parameter is a multiplicative factor
    // applied on the gradient of the metric. Therefore, its effect on the
    // optimizer step length is proportional to the metric values themselves.
    // Metrics with large values will require you to use smaller values for the
    // learning rate in order to maintain a similar optimizer behavior.

    // If this is the first resolution level we set the learning rate
    // (representing the first step size) and the minimum step length (representing
    // the convergence criterion) to large values.  At each subsequent resolution
    // level, we will reduce the minimum step length by a factor of 5 in order to
    // allow the optimizer to focus on progressively smaller regions. The learning
    // rate is set up to the current step length. In this way, when the
    // optimizer is reinitialized at the beginning of the registration process for
    // the next level, the step length will simply start with the last value used
    // for the previous level. This will guarantee the continuity of the path
    // taken by the optimizer through the parameter space.
    if (registration->GetCurrentLevel() == 0)
    {
      optimizer->SetLearningRate(16.00);
      optimizer->SetMinimumStepLength(1); //2.5
    }
    else
    {
      optimizer->SetLearningRate(optimizer->GetCurrentStepLength());
      optimizer->SetMinimumStepLength(optimizer->GetMinimumStepLength() * 0.1); //0.2
    }
  }

  // Define the version of the \code{Execute()} method accepting the input object
  // \code{const itk::Object *}.
  // It is also required since this method is defined as pure virtual
  // in the base class.  This version simply returns without taking any action.
  void
  Execute(const itk::Object *, const itk::EventObject &) override
  {
    return;
  }
};

class RegisterImages: public itk::Object
{
public :
  
  /** Standard class typedefs */
  typedef RegisterImages  Self;
  typedef itk::Object  Superclass;
  typedef itk::SmartPointer< Self >  Pointer;
  typedef itk::SmartPointer< const Self >  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro( Self );

  /** Run-time type information (and related methods). */
  itkTypeMacro( RegisterImages, itk::Object );

  /** Some typedefs */
  //typedef itk::Image< unsigned short, 3 >  ImageType;
  typedef itk::Image< double, 3 >  ImageType;
  typedef itk::VersorRigid3DTransform< double>  TransformType;
  typedef itk::RegularStepGradientDescentOptimizerv4<double> OptimizerType;
  typedef OptimizerType::ScalesType OptimizerScalesType;
  typedef itk::ImageRegistrationMethodv4<ImageType, ImageType, TransformType> RegistrationType;
  typedef itk::MattesMutualInformationImageToImageMetricv4<ImageType, ImageType> MetricType;
  // Read  
  void ReadImages(const char* fileNameT1, const char* fileNameT2);

  //Initialize the registration
  void InitializeTransform();

  void RunRegistration();

  void WriteOutResults(std::string outImageFile);

  void WriteOutputImage(std::string outImageFile, ImageType::Pointer resampledImage);

  void SetTranslationScale(double transScale)
  {
    m_TranslationScale = transScale;
  }

  void SetNumberOfIterations(int numIter)
  {
    m_NumberOfIterations = numIter;
  }

  void SetNumberOfLevels(int numLevels)
  {
    m_NumberOfLevels = numLevels;
  }

  void SetNumberOfHistogramBins(int numLevels)
  {
    m_NumberOfHistogramBins = numLevels;
  }

  void SetBackgroundGrayLevel(double bgLevel)
  {
    m_BackgroundGrayLevel = bgLevel;
  }

  void SetSmoothingSigmas(double sigma)
  {
    m_SmoothingSigmas = sigma;
  }

  void SetCenterOfMassInit(bool useCenterOfMass)
  {
    m_CenterOfMass = useCenterOfMass;
  }

  void SetSamplingPercentage(double sampRate)
  {
    m_SamplingPercentage = sampRate;
  }

  void SetInterpolator(std::string interpolator)
  {
    m_Interpolator = interpolator;
  }

protected:
  RegisterImages();
  virtual ~RegisterImages();


private:
  RegisterImages(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  ImageType::Pointer  m_FixedImage;
  ImageType::Pointer  m_MovingImage;

  TransformType::Pointer m_InitialTransform;  
  
  double m_TranslationScale;
  int m_NumberOfIterations;
  int m_NumberOfHistogramBins;
  int m_NumberOfLevels;
  double m_BackgroundGrayLevel;
  double m_SmoothingSigmas;
  bool m_CenterOfMass;
  double m_SamplingPercentage;
  std::string m_Interpolator;
  TransformType::Pointer m_FinalTransform;
};


} // end namespace kvl

#endif

