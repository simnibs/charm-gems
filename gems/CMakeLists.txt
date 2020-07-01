cmake_minimum_required(VERSION 3.5)
project(gems)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(interfaces)

set(default_build_type "Release")


# find ITK if we're building independently of freesurfer
if(NOT ITK_FOUND)
  find_package(ITK REQUIRED)
endif()
include(${ITK_USE_FILE})

# same with zlib
if(NOT ZBLIB_FOUND)
  find_package(ZLIB REQUIRED)
endif()



option(GEMS_BUILD_SHARED_LIBS "Build GEMS with shared libraries" OFF)

set(GEMS_RUNTIME_PATH ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${GEMS_RUNTIME_PATH})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${GEMS_RUNTIME_PATH})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${GEMS_RUNTIME_PATH})

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-fPIC -fpermissive -msse2 -mfpmath=sse")

# source
set(SOURCES
  gzstream.C
  itkMGHImageIO.cxx
  itkMGHImageIOFactory.cxx
  kvlAtlasMeshAlphaDrawer.cxx
  kvlAtlasMeshCollection.cxx
  kvlAtlasMeshCollectionValidator.cxx
  kvlAtlasMeshDeformationConjugateGradientOptimizer.cxx
  kvlAtlasMeshDeformationFixedStepGradientDescentOptimizer.cxx
  kvlAtlasMeshDeformationGradientDescentOptimizer.cxx
  kvlAtlasMeshDeformationLBFGSOptimizer.cxx
  kvlAtlasMeshDeformationOptimizer.cxx
  kvlAtlasMeshLabelImageStatisticsCollector.cxx
  kvlAtlasMeshMultiAlphaDrawer.cxx
  kvlAtlasMeshPositionCostAndGradientCalculator.cxx
  kvlAtlasMeshProbabilityImageStatisticsCollector.cxx
  kvlAtlasMeshRasterizor.cxx
  kvlAtlasMeshSmoother.cxx
  kvlAtlasMeshStatisticsCollector.cxx
  kvlAtlasMeshSummaryDrawer.cxx
  kvlAtlasMeshToIntensityImageCostAndGradientCalculator.cxx
  kvlAtlasMeshToLabelImageCostAndGradientCalculator.cxx
  kvlAtlasMeshToPointSetCostAndGradientCalculator.cxx
  kvlAtlasMeshVisitCounter.cxx
  kvlAtlasMeshValueDrawer.cxx
  kvlAtlasParameterEstimator.cxx
  kvlAverageAtlasMeshPositionCostAndGradientCalculator.cxx
  kvlCompressionLookupTable.cxx
  kvlConditionalGaussianEntropyCostAndGradientCalculator.cxx
  kvlCroppedImageReader.cxx
  kvlHistogrammer.cxx
  kvlMutualInformationCostAndGradientCalculator.cxx
  kvlAtlasMeshJacobianDeterminantDrawer.cxx
)

# gems libary
add_library(kvlGEMSCommon ${SOURCES})
target_link_libraries(kvlGEMSCommon PRIVATE ${ITK_LIBRARIES} PRIVATE ${ZLIB_LIBRARIES})

