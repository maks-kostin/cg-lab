#pragma once
#include <QtCore/QCoreApplication>
#include <QImage>
#include <string.h> 
#include <algorithm>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>

const double PI = 3.14159265;

class Filter
{
protected:
	virtual QColor calcNewPixelColor(const QImage& img, int x, int y) const = 0;
public:
	virtual ~Filter() = default;
	virtual QImage process(const QImage& img) const;
};

class Kernel
{
protected:
	std::unique_ptr<float[]> data;
	std::size_t radius;
	std::size_t getLen() const
	{
		return getSize() * getSize();
	}
public:
	Kernel(std::size_t radius) : radius(radius)
	{
		data = std::make_unique<float[]>(getLen());
	}

	Kernel(const Kernel& other) : Kernel(other.radius)
	{
		std::copy(other.data.get(), other.data.get() + getLen(), data.get());
	}
	std::size_t getRadius()const
	{
		return radius;
	}
	std::size_t getSize() const
	{
		return 2 * radius + 1;
	}
	void SetKernel(float* dataK, int rad)
	{
		radius = rad;
		std::size_t len = getLen();
		if (data)
		{
			data.reset();
		}
		data = std::make_unique<float[]>(len);
		std::copy(dataK, dataK + len, data.get());
	}
	float operator [](std::size_t id) const
	{
		return data[id];
	}
	float& operator [] (std::size_t id)
	{
		return data[id];
	}
};

class MatrixFilter : public Filter
{
protected:
	Kernel mKernel;
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
public:
	MatrixFilter(const Kernel& kernel) : mKernel(kernel) {};
	virtual ~MatrixFilter() = default;
};

class BlurKernel : public Kernel
{
public:
	using Kernel::Kernel;
	BlurKernel(std::size_t radius = 2) : Kernel(radius)
	{
		for (std::size_t i = 0; i < getLen(); i++)
			data[i] = 1.0f / getLen();
	}
};

class BlurFilter : public MatrixFilter
{
public:
	BlurFilter(std::size_t radius = 1) : MatrixFilter(BlurKernel(radius)) {}
};

class GaussianKernel : public Kernel
{
public:
	using Kernel::Kernel;
	GaussianKernel(std::size_t radius = 2, float sigma = 3.f) : Kernel(radius)
	{
		float norm = 0;
		int signed_radius = static_cast<int>(radius);
		for (int x = -signed_radius; x <= signed_radius; x++)
		{
			for (int y = -signed_radius; y <= signed_radius; y++)
			{
				std::size_t idx = (x + radius) * getSize() + (y + radius);
				data[idx] = std::exp(-(x * x + y * y) / (sigma * sigma));
				norm += data[idx];
			}
		}
		for (std::size_t i = 0; i < getLen(); i++)
		{
			data[i] /= norm;
		}
	}
};

class GaussianFilter : public MatrixFilter
{
public:
	GaussianFilter(std::size_t radius = 2) : MatrixFilter(GaussianKernel(radius)) {}
};

class InvertFilter : public Filter
{
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
};

class GrayScaleFilter : public Filter
{
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
};

class Sepia : public Filter
{
protected:
	int k;
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
public:
	Sepia(int _k = 20)
	{
		k = _k;
	}
};

class Brightness : public Filter
{
protected:
	int C;
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
public:
	Brightness(int _C = 30)
	{
		C = _C;
	}
};

class SobelX : public Kernel
{
public:
	using Kernel::Kernel;
	SobelX() : Kernel(1)
	{
		data[0] = -1.f;
		data[1] = 0.f;
		data[2] = 1.f;
		data[3] = -2.f;
		data[4] = 0.f;
		data[5] = 2.f;
		data[6] = -1.f;
		data[7] = 0.f;
		data[8] = 1.f;
	}
};

class SobelY : public Kernel
{
public:
	using Kernel::Kernel;
	SobelY() : Kernel(1)
	{
		data[0] = -1.f;
		data[1] = -2.f;
		data[2] = -1.f;
		data[3] = 0.f;
		data[4] = 0.f;
		data[5] = 0.f;
		data[6] = 1.f;
		data[7] = 2.f;
		data[8] = 1.f;
	}
};

class SobelMatrixX : public MatrixFilter
{
public:
	SobelMatrixX() : MatrixFilter(SobelX()) {}
};

class SobelMatrixY : public MatrixFilter
{
public:
	SobelMatrixY() : MatrixFilter(SobelY()) {}
};

class SharpnessKernel : public Kernel
{
public:
	using Kernel::Kernel;
	SharpnessKernel() : Kernel(1)
	{
		data[0] = 0.f;
		data[1] = -1.f;
		data[2] = 0.f;
		data[3] = -1.f;
		data[4] = 5.f;
		data[5] = -1.f;
		data[6] = 0.f;
		data[7] = -1.f;
		data[8] = 0.f;
	}
};

class Sharpness : public MatrixFilter
{
public:
	Sharpness() : MatrixFilter(SharpnessKernel()) {}
};

class GreyWorld : public Filter
{
protected:
	double Rs, Gs, Bs, AVG;
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
public:
	QImage process(const QImage& img);
};

class LinealStretching : public Filter
{
protected:
	float maxR, maxG, maxB;
	float minR, minG, minB;
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
public:
	LinealStretching(const QImage& img)
	{
		float tmpR = 0, tmpG = 0, tmpB = 0;
		maxR = 0; maxG = 0; maxB = 0;
		minR = 255; minG = 255; minB = 255;
		for (int x = 0; x < img.width(); x++)
			for (int y = 0; y < img.height(); y++)
			{
				QColor color = img.pixelColor(x, y);
				tmpR = color.red();
				tmpG = color.green();
				tmpB = color.blue();
				if (tmpR > maxR)
					maxR = tmpR;
				if (tmpG > maxG)
					maxG = tmpG;
				if (tmpB > maxB)
					maxB = tmpB;
				if (tmpR < minR)
					minR = tmpR;
				if (tmpG < minG)
					minG = tmpG;
				if (tmpB < minB)
					minB = tmpB;
			}
	}
};

class HorizontalWaves :public Filter
{
	QColor calcNewPixelColor(const QImage& img, int x, int y)  const override;
};

class VerticalWaves : public Filter
{
	QColor calcNewPixelColor(const QImage& img, int x, int y)  const override;
};

class Glass : public Filter
{
protected:
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
};

class Transfer : public Filter
{
protected:
	int x1, y1;
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
public:
	Transfer(int _x1 = 50, int _y1 = 0)
	{
		x1 = _x1;
		y1 = _y1;
	}
};

class DilationKernel : public Kernel
{
public:
	using Kernel::Kernel;
	DilationKernel(std::size_t radius = 1) : Kernel(radius)
	{
		for (int i = 0; i < getLen(); i++)
			data[i] = 1;
	}
};

class Dilation : public MatrixFilter
{
protected:
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
public:
	Dilation(std::size_t radius = 1) :MatrixFilter(DilationKernel(radius)) {}
	Dilation(Kernel& ker) :MatrixFilter(ker) {}
};

class ErosionKernel : public Kernel
{
public:
	using Kernel::Kernel;
	ErosionKernel(std::size_t radius = 1) : Kernel(radius)
	{
		for (int i = 0; i < getLen(); i++)
			data[i] = 1;
	}
};

class Erosion : public MatrixFilter
{
protected:
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
public:
	Erosion(std::size_t radius = 1) : MatrixFilter(ErosionKernel(radius)) {}
	Erosion(Kernel& ker) :MatrixFilter(ker) {}
};

class OpeningKernel : public Kernel
{
public:
	using Kernel::Kernel;
	OpeningKernel(std::size_t radius = 1) : Kernel(radius)
	{

		for (int i = 0; i < getLen(); i++)
			data[i] = 1;
	}
};

class Opening : public MatrixFilter
{
protected:
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
public:
	Opening(std::size_t radius = 1) : MatrixFilter(OpeningKernel(radius)) {}
	Opening(Kernel& ker) :MatrixFilter(ker) {}
	QImage process(const QImage& img) const;
};

class ClosingKernel : public Kernel
{
public:
	using Kernel::Kernel;
	ClosingKernel(std::size_t radius = 1) : Kernel(radius)
	{
		for (int i = 0; i < getLen(); i++)
			data[i] = 1;
	}
};

class Closing : public MatrixFilter
{
protected:
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
public:
	Closing(std::size_t radius = 1) : MatrixFilter(ClosingKernel(radius)) {}
	Closing(Kernel& ker) :MatrixFilter(ker) {}
	QImage process(const QImage& img) const;
};

class GradKernel : public Kernel
{
public:
	using Kernel::Kernel;
	GradKernel(std::size_t radius = 1) : Kernel(radius)
	{
		for (int i = 0; i < getLen(); i++)
			data[i] = 1;
	}
};

class Grad : public MatrixFilter
{
protected:
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
public:
	Grad(std::size_t radius = 1) : MatrixFilter(GradKernel(radius)) {}
	Grad(Kernel& ker) :MatrixFilter(ker) {}
	QImage process(const QImage& img) const;
};

class MedianKernel : public Kernel
{
public:
	using Kernel::Kernel;
	MedianKernel(std::size_t radius = 1) : Kernel(radius)
	{
		for (int i = 0; i < getLen(); i++)
			data[i] = 1;
	}
};

class Median : public MatrixFilter
{
protected:
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
public:
	Median(std::size_t radius = 1) : MatrixFilter(MedianKernel(radius)) {}
};

class MotionBlurKernel : public Kernel
{
public:
	using Kernel::Kernel;
	MotionBlurKernel(std::size_t radius = 1) : Kernel(radius)
	{
		int k = 0;
		for (int i = 0; i < getLen(); i++)
		{
			if (i == k)
			{
				data[i] = 1.0 / getSize();
				k = k + getSize() + 1;
			}
			else
				data[i] = 0;
		}
	}
};

class MotionBlur : public MatrixFilter
{
public:
	MotionBlur(std::size_t radius = 1) : MatrixFilter(MotionBlurKernel(radius)) {}
};



