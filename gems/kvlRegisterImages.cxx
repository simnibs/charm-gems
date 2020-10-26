#include "kvlRegisterImages.h"
#include "itkImageFileReader.h"
#include "itkCenteredTransformInitializer.h"
#include "itkResampleImageFilter.h"
#include "itkBSplineInterpolateImageFunction.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkCastImageFilter.h"
#include "itkImageFileWriter.h"
#include "itkNormalizeImageFilter.h"


namespace kvl
{

//
//
//
RegisterImages
::RegisterImages()
{
    m_FixedImage = 0;
    m_MovingImage = 0;
    m_InitialTransform = TransformType::New();
    m_InitialTransform->SetIdentity();

    m_TranslationScale = 0.0001;
    m_NumberOfIterations = 100;
    m_NumberOfHistogramBins = 50;
    m_NumberOfLevels = 3;
    m_BackgroundGrayLevel = 0;
    m_SmoothingSigmas = 1;
    m_CenterOfMass = false;
    m_SamplingPercentage = 0.5;
    m_Interpolator = "b";
    m_FinalTransform = 0;
}


void RegisterImages
::ReadImages(const char* fileNameT1, const char* fileNameT2)
{
    //Read in the T1 image
    typedef itk::ImageFileReader< ImageType >  ReaderType;
    ReaderType::Pointer  fixedImageReader = ReaderType::New();
    fixedImageReader->SetFileName( fileNameT1 );
    fixedImageReader->Update(); 
    //Set-up the fixed image
    m_FixedImage = fixedImageReader->GetOutput();

    //Then the T2
    ReaderType::Pointer  movingImageReader = ReaderType::New();
    movingImageReader->SetFileName( fileNameT2 );
    movingImageReader->Update();
    //Set-up the moving image
    m_MovingImage = movingImageReader->GetOutput();
}

void RegisterImages
::InitializeTransform()
{
    typedef itk::CenteredTransformInitializer<TransformType, ImageType, ImageType> TransformInitializerType;
    TransformInitializerType::Pointer initializer = TransformInitializerType::New();

    initializer->SetTransform(m_InitialTransform);
    initializer->SetFixedImage(m_FixedImage);
    initializer->SetMovingImage(m_MovingImage);

    if(m_CenterOfMass)
    {
        initializer->MomentsOn();
    }
    else
    {
        initializer->GeometryOn();
    }
    initializer->InitializeTransform();

    TransformType::MatrixType initialMatrix = m_InitialTransform->GetMatrix();
    TransformType::OffsetType initialOffset = m_InitialTransform->GetOffset();
    std::cout << "Initial matrix = " << std::endl
              << initialMatrix << std::endl;
    std::cout << "Initial offset = " << std::endl
              << initialOffset << "\n"
              << std::endl;
}

void RegisterImages
::RunRegistration()
{
    //Set-up registration and optimization
    RegistrationType::Pointer registration = RegistrationType::New();
    OptimizerType::Pointer optimizer = OptimizerType::New();
    registration->SetOptimizer(optimizer);

    //And the metric
    MetricType::Pointer metric = MetricType::New();
    registration->SetMetric(metric);

    //Normalize images
    typedef itk::NormalizeImageFilter<ImageType,ImageType> NormalizeFilterType;
    NormalizeFilterType::Pointer fixedNormalizer = NormalizeFilterType::New();
    NormalizeFilterType::Pointer movingNormalizer = NormalizeFilterType::New();

    fixedNormalizer->SetInput(m_FixedImage);
    movingNormalizer->SetInput(m_MovingImage);
    fixedNormalizer->Update();
    movingNormalizer->Update();

    //Feed the normalized images to the registration
    registration->SetFixedImage(fixedNormalizer->GetOutput());
    registration->SetMovingImage(movingNormalizer->GetOutput());
    
    //Set the initial transform
    registration->SetInitialTransform(m_InitialTransform);

    std::cout << "\nInitial transform: " << std::endl;
    std::cout << "   Moving image: " << registration->GetInitialTransform()->GetParameters() << std::endl;
    std::cout << "   Fixed image:  " << registration->GetInitialTransform()->GetFixedParameters() << "\n"
                << std::endl;
    //Set parameters for the optimizer
    OptimizerScalesType optimizerScales(m_InitialTransform->GetNumberOfParameters());
    optimizerScales[0] = 1.0;
    optimizerScales[1] = 1.0;
    optimizerScales[2] = 1.0;
    optimizerScales[3] = m_TranslationScale;
    optimizerScales[4] = m_TranslationScale;
    optimizerScales[5] = m_TranslationScale;
    optimizer->SetScales(optimizerScales);
    optimizer->SetNumberOfIterations(m_NumberOfIterations);
    optimizer->SetRelaxationFactor(0.5);
    optimizer->SetReturnBestParametersAndValue(true);
    optimizer->SetGradientMagnitudeTolerance(1e-4);
    optimizer->SetMinimumConvergenceValue(1e-4);
    
    //Set sampling strategy
    RegistrationType::MetricSamplingStrategyType samplingStrategy = RegistrationType::RANDOM;
    registration->SetMetricSamplingStrategy(samplingStrategy);
    registration->SetMetricSamplingPercentage(m_SamplingPercentage);

    //Could be skipped if we don't need to test
    constexpr int randomNumberGeneratorSeed = 121213;
    registration->MetricSamplingReinitializeSeed(randomNumberGeneratorSeed);

    metric->SetNumberOfHistogramBins(m_NumberOfHistogramBins);

    //Add the observer
    CommandIterationUpdate::Pointer observer = CommandIterationUpdate::New();
    optimizer->AddObserver(itk::IterationEvent(), observer);

    //Add number of levels
    registration->SetNumberOfLevels(m_NumberOfLevels);
    RegistrationType::ShrinkFactorsArrayType shrinkFactorsPerLevel;
    shrinkFactorsPerLevel.SetSize(m_NumberOfLevels);

    for (auto i = 0; i < m_NumberOfLevels; ++i)
    {
        shrinkFactorsPerLevel[i] = static_cast<unsigned long>(pow(2, m_NumberOfLevels - 1 - i));
    }
    registration->SetShrinkFactorsPerLevel(shrinkFactorsPerLevel);
    
    // Set the smoothing sigmas for each level in terms of voxels.
    registration->SetSmoothingSigmasAreSpecifiedInPhysicalUnits(false);
    RegistrationType::SmoothingSigmasArrayType smoothingSigmasPerLevel;
    smoothingSigmasPerLevel.SetSize(m_NumberOfLevels);
    for (auto i = 0; i < m_NumberOfLevels; ++i)
    {
        smoothingSigmasPerLevel[i] = (m_SmoothingSigmas * shrinkFactorsPerLevel[i]) > 0 ? (m_SmoothingSigmas * shrinkFactorsPerLevel[i]) : 0;     
    }

    registration->SetSmoothingSigmasPerLevel(smoothingSigmasPerLevel);

    // Create the Command observer and register it with the registration.
    typedef RegistrationInterfaceCommand<RegistrationType> CommandType;
    CommandType::Pointer command = CommandType::New();
    registration->AddObserver(itk::MultiResolutionIterationEvent(), command);
    
    //  Trigger the registration process by calling \code{Update()}.
    try
    {
        registration->Update();

        std::string stopCondition{registration->GetOptimizer()->GetStopConditionDescription()};
        std::cout << std::endl;

        if (((stopCondition.find("Step too small") != std::string::npos) || (stopCondition.find("Convergence checker passed at iteration") != std::string::npos)) == false)
        {
            std::cout << "Warning message:" << std::endl;
            std::cout << "   Model failed to converge!" << std::endl;

            if (stopCondition.find("Maximum number of iterations") != std::string::npos)
            {
                std::cout << "   Try to increase the number of iterations by adding the option \"-i 500\" by the end of the command" << std::endl;
            }
        }

        std::cout << "   Optimizer stop condition: " << stopCondition << std::endl;
    }
    catch (itk::ExceptionObject &err)
    {
        std::cout << "ExceptionObject caught !" << std::endl;
        std::cout << err << std::endl;
        exit(EXIT_FAILURE);
    }
    catch (...)
    {
        std::cout << "Caught an non-ITK exception !" << std::endl;
        exit(EXIT_FAILURE);
    }

    const TransformType::ParametersType finalParameters =
      registration->GetOutput()->Get()->GetParameters();
    
    const double versorX = finalParameters[0];
    const double versorY = finalParameters[1];
    const double versorZ = finalParameters[2];
    const double finalTranslationX = finalParameters[3];
    const double finalTranslationY = finalParameters[4];
    const double finalTranslationZ = finalParameters[5];
    const unsigned int currentIterations = optimizer->GetCurrentIteration();
    const double bestValue = optimizer->GetValue();

    // Print out results
    std::cout << "Result = " << std::endl;
    std::cout << "   Versor X      = " << versorX << std::endl;
    std::cout << "   Versor Y      = " << versorY << std::endl;
    std::cout << "   Versor Z      = " << versorZ << std::endl;
    std::cout << "   Translation X = " << finalTranslationX << std::endl;
    std::cout << "   Translation Y = " << finalTranslationY << std::endl;
    std::cout << "   Translation Z = " << finalTranslationZ << std::endl;
    std::cout << "   Iterations    = " << currentIterations << std::endl;
    std::cout << "   Metric value  = " << bestValue << "\n"
              << std::endl;
    
    m_FinalTransform = TransformType::New();
    m_FinalTransform->SetFixedParameters(
      registration->GetOutput()->Get()->GetFixedParameters());
    m_FinalTransform->SetParameters(finalParameters);
}

void RegisterImages
::WriteOutputImage(std::string outImageFile, ImageType::Pointer resampledImage)
{
  typedef itk::CastImageFilter<ImageType, ImageType> CastFilterType;
  typedef itk::ImageFileWriter<ImageType> WriterType;

  WriterType::Pointer writer = WriterType::New();
  CastFilterType::Pointer caster = CastFilterType::New();

  writer->SetFileName(outImageFile);
  caster->SetInput(resampledImage); //resampler->GetOutput()
  writer->SetInput(caster->GetOutput());
  writer->Update();

  std::cout << "\nOutput registered image file: " << writer->GetFileName() << "\n"
            << std::endl;
}

void RegisterImages
::WriteOutResults(std::string outImageFile)
{
    typedef itk::ResampleImageFilter<ImageType, ImageType> ResampleFilterType;
    ResampleFilterType::Pointer resampler = ResampleFilterType::New();
    resampler->SetTransform(m_FinalTransform);
    resampler->SetInput(m_MovingImage);
    resampler->SetSize(m_FixedImage->GetLargestPossibleRegion().GetSize());
    resampler->SetOutputOrigin(m_FixedImage->GetOrigin());
    resampler->SetOutputSpacing(m_FixedImage->GetSpacing());
    resampler->SetOutputDirection(m_FixedImage->GetDirection());
    resampler->SetDefaultPixelValue(m_BackgroundGrayLevel);

    itk::InterpolateImageFunction<ImageType, double>::Pointer interpolator;

    if (m_Interpolator == std::string("l"))
    {
        interpolator = itk::LinearInterpolateImageFunction<ImageType, double>::New();
        std::cout << "linear interpolation." << std::endl;
    }
    else if (m_Interpolator == std::string("n"))
    {
        interpolator = itk::NearestNeighborInterpolateImageFunction<ImageType, double>::New();
        std::cout << "nearest neighbor interpolation." << std::endl;
    }
    else
    {
        typedef itk::BSplineInterpolateImageFunction<ImageType, double> BSplineInterpolatorType;
        BSplineInterpolatorType::Pointer bsplineInterpolator = BSplineInterpolatorType::New();
        bsplineInterpolator->SetSplineOrder(4);

        std::cout << "B-Spline interpolation." << std::endl;
        std::cout << "The BSpline order: " << bsplineInterpolator->GetSplineOrder() << std::endl;

        interpolator = bsplineInterpolator;
    }

    resampler->SetInterpolator(interpolator);

    // I'm fixing the data type to double. Would probably
    // be smart to check the types and stick to the type
    // of the moving images.

   this->WriteOutputImage(outImageFile, resampler->GetOutput());


}

RegisterImages
::~RegisterImages(){}
}