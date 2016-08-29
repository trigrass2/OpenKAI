/*
 * CameraInput.cpp
 *
 *  Created on: Aug 21, 2015
 *      Author: yankai
 */

#include "Frame.h"

namespace kai
{

Frame::Frame()
{
	//Common
	m_CMat.m_frameID = 0;

#ifdef USE_CUDA
	m_GMat.m_frameID = 0;
#endif

}

Frame::~Frame()
{
}

void Frame::getResizedOf(Frame* pFrom, int width, int height)
{
	if(!pFrom)return;
	if(pFrom->empty())return;

	cv::Size newSize = cv::Size(width,height);

	if(newSize == pFrom->getSize())//pFrom->getNextGMat()->size())
	{
		this->update(pFrom);
	}
	else
	{
#ifdef USE_CUDA
		cuda::resize(*pFrom->getGMat(), m_GMat.m_mat, newSize);
		updatedGMat();
#elif USE_OPENCL

#else
		cv::resize(*pFrom->getNextCMat(), m_CMat.m_mat, newSize);
		updatedCMat();
#endif

	}

}

void Frame::getGrayOf(Frame* pFrom)
{
	if(!pFrom)return;

#ifdef USE_CUDA
	if(pFrom->getGMat()->channels()!=3)return;
	cuda::cvtColor(*pFrom->getGMat(), m_GMat.m_mat, CV_BGR2GRAY);//,0, m_cudaStream);
	updatedGMat();
	//	m_cudaStream.waitForCompletion();
#elif USE_OPENCL

#else
	cv::cvtColor(*pFrom->getNextCMat(), m_CMat.m_mat, CV_BGR2GRAY);
	updatedCMat();
#endif

}

void Frame::getHSVOf(Frame* pFrom)
{
	if(!pFrom)return;

#ifdef USE_CUDA
	//RGB or BGR depends on device
	if(pFrom->getGMat()->channels()!=3)return;
	cuda::cvtColor(*pFrom->getGMat(), m_GMat.m_mat, CV_BGR2HSV);
	updatedGMat();
#elif USE_OPENCL

#else
	cv::cvtColor(*pFrom->getNextCMat(), m_CMat.m_mat, CV_BGR2HSV);
	updatedCMat();
#endif

}

void Frame::getBGRAOf(Frame* pFrom)
{
	if(!pFrom)return;

#ifdef USE_CUDA
	if(pFrom->getGMat()->channels()!=3)return;
	cuda::cvtColor(*pFrom->getGMat(), m_GMat.m_mat, CV_BGR2BGRA);
	updatedGMat();
#elif USE_OPENCL

#else
	cv::cvtColor(*pFrom->getNextCMat(), m_CMat.m_mat, CV_BGR2BGRA);
	updatedCMat();
#endif

}

void Frame::get8UC3Of(Frame* pFrom)
{
	if(!pFrom)return;

#ifdef USE_CUDA
	if(pFrom->getGMat()->type()==CV_8UC3)
	{
		pFrom->getGMat()->copyTo(m_GMat.m_mat);
	}
	else
	{
		if(pFrom->getGMat()->channels()!=1)return;
		cuda::cvtColor(*pFrom->getGMat(), m_GMat.m_mat, CV_GRAY2BGR);
	}

	updatedGMat();
#elif USE_OPENCL

#else
	cv::cvtColor(*pFrom->getNextCMat(), m_CMat.m_mat, CV_GRAY2BGR);
	updatedCMat();
#endif

}

uint64_t Frame::getFrameID(void)
{
	uint64_t frameID = m_CMat.m_frameID;

#ifdef USE_CUDA
	if(frameID < m_GMat.m_frameID)frameID = m_GMat.m_frameID;
#elif USE_OPENCL

#endif

	return frameID;
}

bool Frame::empty(void)
{
#ifdef USE_CUDA
	if(m_CMat.m_frameID > m_GMat.m_frameID)
	{
		return m_CMat.m_mat.empty();
	}

	return m_GMat.m_mat.empty();

#elif USE_OPENCL

#else
	return m_CMat.m_mat.empty();
#endif

}

bool Frame::isNewerThan(Frame* pFrame)
{
	if (pFrame == NULL)return false;
	if(pFrame->getFrameID() < this->getFrameID())
	{
		return true;
	}

	return false;
}

void Frame::update(Frame* pFrame)
{
	if (pFrame == NULL)return;

#ifdef USE_CUDA
//	pFrame->getGMat()->copyTo(m_GMat.m_mat);

	m_GMat.m_mat = *pFrame->getGMat();
	updatedGMat();
#elif USE_OPENCL

#else
	pFrame->getCMat()->copyTo(m_CMat.m_mat);
	updatedCMat();
#endif
}

#ifdef USE_CUDA
void Frame::update(GpuMat* pGpuFrame)
{
	if (pGpuFrame == NULL)return;

//	pGpuFrame->copyTo(m_GMat.m_mat);
	m_GMat.m_mat = *pGpuFrame;
	updatedGMat();
}
#endif

void Frame::update(Mat* pFrame)
{
	if (pFrame == NULL)return;

#ifdef USE_CUDA
//	m_GMat.m_mat.upload(*pFrame);
//	updatedGMat();
	m_CMat.m_mat = *pFrame;
	updatedCMat();
#elif USE_OPENCL

#else
	pFrame->copyTo(m_CMat.m_mat);
	updatedCMat();
#endif
}

inline void Frame::updatedCMat(void)
{
	m_CMat.m_frameID = get_time_usec();
}

#ifdef USE_CUDA
void Frame::updatedGMat(void)
{
	m_GMat.m_frameID = get_time_usec();
}
#endif

#ifdef USE_CUDA
GpuMat* Frame::getGMat(void)
{
	//Return the latest content
	if(m_CMat.m_frameID > m_GMat.m_frameID)
	{
		m_GMat.m_mat.upload(m_CMat.m_mat);
		m_GMat.m_frameID = m_CMat.m_frameID;
	}

	return &m_GMat.m_mat;
}
#endif

Mat* Frame::getCMat(void)
{
#ifdef USE_CUDA
	//Return the latest content
	if(m_GMat.m_frameID > m_CMat.m_frameID)
	{
		m_GMat.m_mat.download(m_CMat.m_mat);
		m_CMat.m_frameID = m_GMat.m_frameID;
	}
#endif

	return &m_CMat.m_mat;
}

Size Frame::getSize(void)
{
	Size mySize = Size(0,0);
	mySize = m_CMat.m_mat.size();

#ifdef USE_CUDA
	if(m_GMat.m_frameID > m_CMat.m_frameID)
	{
		mySize = m_GMat.m_mat.size();
	}
#elif USE_OPENCL

#endif

	return mySize;
}








}
