#include "pyKvlAffineRegistration.h"
#include "pyKvlNumpy.h"

py::array_t<double> KvlAffineRegistration::GetFinalTransformation() {
    auto *data = new double[16];
    AffineTransformType::Pointer final_transform = registerer->GetFinalTransformation();
    auto parameters = final_transform->GetParameters();
    for ( unsigned int row = 0; row < 3; row++ )
    {
        for ( unsigned int col = 0; col <  3; col++ )
        {
            data[ col * 4 + row ] = parameters[ row * 3 + col ];
        }
        data[ 12 + row ] = parameters[ 9 + row ];
    }
    for ( unsigned int col = 0; col < 3; col++ )
    {
        data[ col * 4 + 3 ] = 0.0f;
    }
    data[ 15 ] = 1.0f;
    return createNumpyArrayFStyle({4, 4}, data);
}
