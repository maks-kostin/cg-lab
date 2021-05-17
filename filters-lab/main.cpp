#include "filter.h"

int main(int argc, char* argv[])
{
    std::string s;
    QImage img;


    for (int i = 0; i < argc; i++)
    {
        if (!strcmp(argv[i], "-p") && (i + 1 < argc))
            s = argv[i + 1];
    }
    char size[80];
    std::ifstream ifs("KernelM.txt");
    ifs.getline(size, 3, '\n');
    int sizei = std::atoi(size);
    Kernel MatKernel(sizei);
    std::unique_ptr<float[]> tmp = std::make_unique<float[]>(sizei * sizei);
    for (int i = 0; i < sizei * sizei; i++)
    {
        ifs.getline(size, 2, '\n');
        tmp[i] = std::atoi(size);
    }

    MatKernel.SetKernel(tmp.get(), sizei / 2);

    img.load(QString(s.c_str()));
    img.save("Images/Source.png");

    InvertFilter invert;
    invert.process(img).save("C:/Users/Admin/source/repos/filters-lab/filters-lab/Images/Invert.png");

    BlurFilter blur;
    blur.process(img).save("C:/Users/Admin/source/repos/filters-lab/filters-lab/Images/Blur.png");

    GaussianFilter gaussian;
    gaussian.process(img).save("C:/Users/Admin/source/repos/filters-lab/filters-lab/Images/Gaussian.png");

    GrayScaleFilter gray;
    gray.process(img).save("C:/Users/Admin/source/repos/filters-lab/filters-lab/Images/Gray.png");

    Sepia sepia_img;
    sepia_img.process(img).save("C:/Users/Admin/source/repos/filters-lab/filters-lab/Images/Sepia.png");

    Brightness bright;
    bright.process(img).save("C:/Users/Admin/source/repos/filters-lab/filters-lab/Images/Bright.png");

    SobelMatrixX sobelX;
    sobelX.process(img).save("C:/Users/Admin/source/repos/filters-lab/filters-lab/Images/SobelX.png");

    SobelMatrixY sobelY;
    sobelY.process(img).save("C:/Users/Admin/source/repos/filters-lab/filters-lab/Images/SobelY.png");

    Sharpness sharp;
    sharp.process(img).save("C:/Users/Admin/source/repos/filters-lab/filters-lab/Images/Sharpness.png");

    GreyWorld grey;
    grey.process(img).save("C:/Users/Admin/source/repos/filters-lab/filters-lab/Images/GreyWorld.png");

    LinealStretching lineal(img);
    lineal.process(img).save("C:/Users/Admin/source/repos/filters-lab/filters-lab/Images/LinealStretching.png");

    HorizontalWaves hwaves;
    //hwaves.process(img).save("C:/Users/Admin/source/repos/filters-lab/filters-lab/Images/HorizontalWaves.png");

    VerticalWaves vwaves;
    //vwaves.process(img).save("C:/Users/Admin/source/repos/filters-lab/filters-lab/Images/VerticalWaves.png");

    Glass gl;
    gl.process(img).save("C:/Users/Admin/source/repos/filters-lab/filters-lab/Images/Glass.png");

    Transfer transf;
    transf.process(img).save("C:/Users/Admin/source/repos/filters-lab/filters-lab/Images/Transfer.png");

    Dilation dil(MatKernel);
    dil.process(img).save("C:/Users/Admin/source/repos/filters-lab/filters-lab/Images/Dilation.png");

    Erosion eros(MatKernel);
    eros.process(img).save("C:/Users/Admin/source/repos/filters-lab/filters-lab/Images/Erosion.png");

    Opening op(MatKernel);
    op.process(img).save("C:/Users/Admin/source/repos/filters-lab/filters-lab/Images/Opening.png");

    Closing cl(MatKernel);
    cl.process(img).save("C:/Users/Admin/source/repos/filters-lab/filters-lab/Images/Closing.png");

    Grad grad(MatKernel);
    grad.process(img).save("C:/Users/Admin/source/repos/filters-lab/filters-lab/Images/Gradient.png");

    Median med;
    med.process(img).save("C:/Users/Admin/source/repos/filters-lab/filters-lab/Images/Median.png");

    MotionBlur motblur;
    motblur.process(img).save("Images/MotionBlur.png");

    return 0;


}