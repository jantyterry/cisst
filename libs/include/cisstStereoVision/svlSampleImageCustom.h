/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: $
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2010

  (C) Copyright 2006-2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _svlSampleImageCustom_h
#define _svlSampleImageCustom_h

#include <cisstVector/vctDynamicMatrixRef.h>
#include <cisstStereoVision/svlTypeCheckers.h>
#include <cisstStereoVision/svlProcInfo.h>
#include <cisstStereoVision/svlSampleImage.h>
#include <cisstStereoVision/svlSampleMatrix.h>
#include <cisstStereoVision/svlImageIO.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


template <class _ValueType, unsigned int _DataChannels, unsigned int _VideoChannels>
class CISST_EXPORT svlSampleImageCustom : public svlSampleImage
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:

    //////////////////
    // Constructors //
    //////////////////

    svlSampleImageCustom() :
        svlSampleImage(),
        OwnData(true)
    {
        for (unsigned int vch = 0; vch < _VideoChannels; vch ++) {
            OwnImage[vch] = new vctDynamicMatrix<_ValueType>;
            Image[vch].SetRef(*(OwnImage[vch]));
#if (CISST_SVL_HAS_OPENCV == ON)
            int ocvdepth = GetOCVDepth();
            if (ocvdepth >= 0) OCVImageHeader[vch] = cvCreateImageHeader(cvSize(0, 0), ocvdepth, _DataChannels);
            else OCVImageHeader[vch] = 0;
#endif // CISST_SVL_HAS_OPENCV
        }
    }

    svlSampleImageCustom(bool owndata) :
        svlSampleImage(),
        OwnData(owndata)
    {
        for (unsigned int vch = 0; vch < _VideoChannels; vch ++) {
            if (OwnData) {
                OwnImage[vch] = new vctDynamicMatrix<_ValueType>;
                Image[vch].SetRef(*(OwnImage[vch]));
            }
            else {
                OwnImage[vch] = 0;
            }
#if (CISST_SVL_HAS_OPENCV == ON)
            int ocvdepth = GetOCVDepth();
            if (ocvdepth >= 0) OCVImageHeader[vch] = cvCreateImageHeader(cvSize(0, 0), ocvdepth, _DataChannels);
            else OCVImageHeader[vch] = 0;
#endif // CISST_SVL_HAS_OPENCV
        }
    }

    svlSampleImageCustom(const svlSampleImageCustom<_ValueType, _DataChannels, _VideoChannels> & other) :
        svlSampleImage(other),
        OwnData(true)
    {
        for (unsigned int vch = 0; vch < _VideoChannels; vch ++) {
            OwnImage[vch] = new vctDynamicMatrix<_ValueType>;
            Image[vch].SetRef(*(OwnImage[vch]));
#if (CISST_SVL_HAS_OPENCV == ON)
            int ocvdepth = GetOCVDepth();
            if (ocvdepth >= 0) OCVImageHeader[vch] = cvCreateImageHeader(cvSize(0, 0), ocvdepth, _DataChannels);
            else OCVImageHeader[vch] = 0;
#endif // CISST_SVL_HAS_OPENCV
        }
        CopyOf(other);
    }


    ////////////////
    // Destructor //
    ////////////////

    ~svlSampleImageCustom()
    {
        for (unsigned int vch = 0; vch < _VideoChannels; vch ++) {
            if (OwnImage[vch]) delete OwnImage[vch];
#if (CISST_SVL_HAS_OPENCV == ON)
            if (OCVImageHeader[vch]) cvReleaseImageHeader(&(OCVImageHeader[vch]));
#endif // CISST_SVL_HAS_OPENCV
        }
    }

    ///////////////
    // Operators //
    ///////////////

    svlSampleImageCustom & operator= (const svlSampleImageCustom<_ValueType, _DataChannels, _VideoChannels> & other)
    {
        CopyOf(other);
        return *this;
    }


    //////////////////////////////
    // Inherited from svlSample //
    //////////////////////////////

    svlSample* GetNewInstance() const
    {
        return new svlSampleImageCustom<_ValueType, _DataChannels, _VideoChannels>;
    }

    svlStreamType GetType() const
    {
        if (IsTypeUInt8<_ValueType>(static_cast<_ValueType>(0))) {
            if (_DataChannels == 1) {
                if (_VideoChannels == 1) return svlTypeImageMono8;
                if (_VideoChannels == 2) return svlTypeImageMono8Stereo;
            }
            if (_DataChannels == 3) {
                if (_VideoChannels == 1) return svlTypeImageRGB;
                if (_VideoChannels == 2) return svlTypeImageRGBStereo;
            }
            if (_DataChannels == 4) {
                if (_VideoChannels == 1) return svlTypeImageRGBA;
                if (_VideoChannels == 2) return svlTypeImageRGBAStereo;
            }
        }
        if (IsTypeUInt16<_ValueType>(static_cast<_ValueType>(0))) {
            if (_DataChannels == 1) {
                if (_VideoChannels == 1) return svlTypeImageMono16;
                if (_VideoChannels == 2) return svlTypeImageMono16Stereo;
            }
        }
        if (IsTypeFloat<_ValueType>(static_cast<_ValueType>(0))) {
            if (_DataChannels == 3 && _VideoChannels == 1) return svlTypeImage3DMap;
        }
        return svlTypeInvalid;
    }

    int SetSize(const svlSample* sample)
    {
        const svlSampleImage* sampleimage = dynamic_cast<const svlSampleImage*>(sample);
        if (sampleimage) {
            unsigned int samplevideochannels = sampleimage->GetVideoChannels();
            for (unsigned int vch = 0; vch < _VideoChannels && vch < samplevideochannels; vch ++) {
                SetSize(vch, sampleimage->GetWidth(vch), sampleimage->GetHeight(vch));
            }
            return SVL_OK;
        }
        else {
            const svlSampleMatrix* samplematrix = dynamic_cast<const svlSampleMatrix*>(sample);
            if (samplematrix) {
                SetSize(samplematrix->GetCols(), samplematrix->GetRows());
                return SVL_OK;
            }
        }
        return SVL_FAIL;
    }
    
    int SetSize(const svlSample& sample)
    {
        const svlSampleImage* sampleimage = dynamic_cast<const svlSampleImage*>(&sample);
        if (sampleimage) {
            unsigned int samplevideochannels = sampleimage->GetVideoChannels();
            for (unsigned int vch = 0; vch < _VideoChannels && vch < samplevideochannels; vch ++) {
                SetSize(vch, sampleimage->GetWidth(vch), sampleimage->GetHeight(vch));
            }
            return SVL_OK;
        }
        else {
            const svlSampleMatrix* samplematrix = dynamic_cast<const svlSampleMatrix*>(&sample);
            if (samplematrix) {
                SetSize(samplematrix->GetCols(), samplematrix->GetRows());
                return SVL_OK;
            }
        }
        return SVL_FAIL;
    }

    int CopyOf(const svlSample* sample)
    {
        if (!sample) return SVL_FAIL;
        if (sample->GetType() != GetType() || SetSize(sample) != SVL_OK) return SVL_FAIL;

        const svlSampleImage* sampleimage = dynamic_cast<const svlSampleImage*>(sample);
        for (unsigned int vch = 0; vch < _VideoChannels; vch ++) {
            memcpy(GetUCharPointer(vch), sampleimage->GetUCharPointer(vch), GetDataSize(vch));
        }
        SetTimestamp(sample->GetTimestamp());

        return SVL_OK;
    }

    int CopyOf(const svlSample& sample)
    {
        if (sample.GetType() != GetType() || SetSize(sample) != SVL_OK) return SVL_FAIL;

        const svlSampleImage* sampleimage = dynamic_cast<const svlSampleImage*>(&sample);
        for (unsigned int vch = 0; vch < _VideoChannels; vch ++) {
            memcpy(GetUCharPointer(vch), sampleimage->GetUCharPointer(vch), GetDataSize(vch));
        }
        SetTimestamp(sample.GetTimestamp());

        return SVL_OK;
    }

    bool IsInitialized() const
    {
        for (unsigned int vch = 0; vch < _VideoChannels; vch ++) {
            if (Image[vch].width() < _DataChannels ||
                Image[vch].height() < 1) return false;
        }
        return true;
    }

    unsigned char* GetUCharPointer()
    {
        return reinterpret_cast<unsigned char*>(GetPointer());
    }

    const unsigned char* GetUCharPointer() const
    {
        return reinterpret_cast<const unsigned char*>(GetPointer());
    }

    unsigned int GetDataSize() const
    {
        return GetDataSize(0);
    }

    virtual void SerializeRaw(std::ostream & outputStream) const
    {
        mtsGenericObject::SerializeRaw(outputStream);

        std::string codec;
        int compression;
        GetEncoder(codec, compression);
        cmnSerializeRaw(outputStream, GetType());
        cmnSerializeRaw(outputStream, GetTimestamp());
        cmnSerializeRaw(outputStream, codec);

        int hasdata = 1;
        for (unsigned int vch = 0; vch < _VideoChannels; vch ++) {
            if (GetWidth(vch) < 1 || GetHeight(vch) < 1) hasdata = 0;
        }
        cmnSerializeRaw(outputStream, hasdata);

        if (hasdata == 1) {
            for (unsigned int vch = 0; vch < _VideoChannels; vch ++) {
                if (svlImageIO::Write(*this, vch, codec, outputStream, compression) != SVL_OK) {
// Exeptions are not handled yet.
// TO DO: Restore code once exception handling is corrected.
//                    cmnThrow("svlSampleImageCustom::SerializeRaw(): Error occured with svlImageIO::Write");
                }
            }
        }
    }

    virtual void DeSerializeRaw(std::istream & inputStream)
    {
        mtsGenericObject::DeSerializeRaw(inputStream);

        int type = -1;
        double timestamp;
        std::string codec;
        int hasdata;
        cmnDeSerializeRaw(inputStream, type);
        if (type != GetType()) {
            CMN_LOG_CLASS_RUN_ERROR << "Deserialized sample type mismatch " << std::endl;
            return;
        }
        cmnDeSerializeRaw(inputStream, timestamp);
        SetTimestamp(timestamp);
        cmnDeSerializeRaw(inputStream, codec);
        cmnDeSerializeRaw(inputStream, hasdata);

        if (hasdata == 1) {
            for (unsigned int vch = 0; vch < _VideoChannels; vch ++) {
                if (svlImageIO::Read(*this, vch, codec, inputStream, false) != SVL_OK) {
// Exeptions are not handled yet.
// TO DO: Restore code once exception handling is corrected.
//                    cmnThrow("svlSampleImageCustom::DeSerializeRaw(): Error occured with svlImageIO::Read");
                }
            }
        }
    }

    ///////////////////////////////////
    // Inherited from svlSampleImage //
    ///////////////////////////////////

    virtual svlPixelType GetPixelType() const
    {
        switch (GetType()) {
            case svlTypeImageRGB:
            case svlTypeImageRGBStereo:
                return svlPixelRGB;
            break;

            case svlTypeImageRGBA:
            case svlTypeImageRGBAStereo:
                return svlPixelRGBA;
            break;

            case svlTypeImageMono8:
            case svlTypeImageMono8Stereo:
                return svlPixelMono8;
            break;

            case svlTypeImageMono16:
            case svlTypeImageMono16Stereo:
                return svlPixelMono16;
            break;

            case svlTypeImage3DMap:
                return svlPixel3DFloat;
            break;

            case svlTypeMatrixInt8:
            case svlTypeMatrixInt16:
            case svlTypeMatrixInt32:
            case svlTypeMatrixInt64:
            case svlTypeMatrixUInt8:
            case svlTypeMatrixUInt16:
            case svlTypeMatrixUInt32:
            case svlTypeMatrixUInt64:
            case svlTypeMatrixFloat:
            case svlTypeMatrixDouble:
            case svlTypeInvalid:
            case svlTypeStreamSource:
            case svlTypeStreamSink:
            case svlTypeTransform3D:
            case svlTypeTargets:
            case svlTypeText:
            break;
        }
        return svlPixelUnknown;
    }

#if (CISST_SVL_HAS_OPENCV == ON)
    IplImage* IplImageRef(const unsigned int videochannel = 0) const
#else // CISST_SVL_HAS_OPENCV
    IplImage* IplImageRef(const unsigned int CMN_UNUSED(videochannel) = 0) const
#endif // CISST_SVL_HAS_OPENCV
    {
#if (CISST_SVL_HAS_OPENCV == ON)
        if (videochannel < _VideoChannels) return OCVImageHeader[videochannel];
        else return 0;
#else // CISST_SVL_HAS_OPENCV
        CMN_LOG_CLASS_INIT_ERROR << "Class svlSampleImageCustom: IplImageRef() called while OpenCV is disabled" << std::endl;
        return 0;
#endif // CISST_SVL_HAS_OPENCV
    }

    unsigned char* GetUCharPointer(const unsigned int videochannel)
    {
        return reinterpret_cast<unsigned char*>(GetPointer(videochannel));
    }

    const unsigned char* GetUCharPointer(const unsigned int videochannel) const
    {
        return reinterpret_cast<const unsigned char*>(GetPointer(videochannel));
    }

    unsigned char* GetUCharPointer(const unsigned int videochannel, const unsigned int x, const unsigned int y)
    {
        return reinterpret_cast<unsigned char*>(GetPointer(videochannel, x, y));
    }

    const unsigned char* GetUCharPointer(const unsigned int videochannel, const unsigned int x, const unsigned int y) const
    {
        return reinterpret_cast<const unsigned char*>(GetPointer(videochannel, x, y));
    }

    void SetSize(const unsigned int width, const unsigned int height)
    {
        for (unsigned int vch = 0; vch < _VideoChannels; vch ++) {
            SetSize(vch, width, height);
        }
    }

    void SetSize(const unsigned int videochannel, const unsigned int width, const unsigned int height)
    {
        if (OwnData && videochannel < _VideoChannels) {
            if (GetWidth (videochannel) == width &&
                GetHeight(videochannel) == height) return;
            OwnImage[videochannel]->SetSize(height,  width * _DataChannels);
            Image[videochannel].SetRef(*(OwnImage[videochannel]));
#if (CISST_SVL_HAS_OPENCV == ON)
            if (OCVImageHeader[videochannel]) {
                cvInitImageHeader(OCVImageHeader[videochannel],
                                  cvSize(width, height),
                                  GetOCVDepth(),
                                  _DataChannels);
                cvSetData(OCVImageHeader[videochannel],
                          GetPointer(videochannel),
                          width * GetBPP());
            }
#endif // CISST_SVL_HAS_OPENCV
        }
    }

    unsigned int GetVideoChannels() const
    {
        return _VideoChannels;
    }

    unsigned int GetDataChannels() const
    {
        return _DataChannels;
    }

    unsigned int GetBPP() const
    {
        return (sizeof(_ValueType) * _DataChannels);
    }

    int GetAlphaChannel() const
    {
        switch (GetType()) {
            case svlTypeImageRGBA:
            case svlTypeImageRGBAStereo:
                return 3;
            break;

            case svlTypeImageMono8:
            case svlTypeImageMono8Stereo:
            case svlTypeImageMono16:
            case svlTypeImageMono16Stereo:
            case svlTypeImage3DMap:
            case svlTypeImageRGB:
            case svlTypeImageRGBStereo:
            case svlTypeMatrixInt8:
            case svlTypeMatrixInt16:
            case svlTypeMatrixInt32:
            case svlTypeMatrixInt64:
            case svlTypeMatrixUInt8:
            case svlTypeMatrixUInt16:
            case svlTypeMatrixUInt32:
            case svlTypeMatrixUInt64:
            case svlTypeMatrixFloat:
            case svlTypeMatrixDouble:
            case svlTypeInvalid:
            case svlTypeStreamSource:
            case svlTypeStreamSink:
            case svlTypeTransform3D:
            case svlTypeTargets:
            case svlTypeText:
            break;
        }
        return SVL_FAIL;
    }

    unsigned int GetWidth(const unsigned int videochannel = 0) const
    {
        if (videochannel < _VideoChannels) return (Image[videochannel].width() / _DataChannels);
        return 0;
    }

    unsigned int GetHeight(const unsigned int videochannel = 0) const
    {
        if (videochannel < _VideoChannels) return Image[videochannel].height();
        return 0;
    }

    unsigned int GetRowStride(const unsigned int videochannel = 0) const
    {
        if (videochannel < _VideoChannels) return Image[videochannel].width();
        return 0;
    }

    unsigned int GetDataSize(const unsigned int videochannel) const
    {
        if (videochannel < _VideoChannels) {
            return (GetBPP() * GetWidth(videochannel) * GetHeight(videochannel));
        }
        return 0;
    }

    svlSampleImage* GetSubImage(const unsigned int top, const unsigned int height, const unsigned int videochannel = 0)
    {
        if (videochannel < _VideoChannels) {

            const unsigned int width = GetWidth(videochannel);

            // Create sub-matrix reference
            vctDynamicMatrixRef<_ValueType> subref(Image[videochannel], top, 0, height, width);

            // Create sub-sample image
            svlSampleImageCustom<_ValueType, _DataChannels, 1>* subimage = new svlSampleImageCustom<_ValueType, _DataChannels, 1>(false);

            // Set sub-sample image to sub-matrix reference
            subimage->SetMatrix(subref);

            return subimage;
        }
        return 0;
    }

    svlSampleImage* GetSubImage(svlProcInfo* procInfo, const unsigned int videochannel = 0)
    {
        if (videochannel < _VideoChannels) {

            unsigned int height = GetHeight(videochannel);
            unsigned int from, to;

            _GetParallelSubRange(procInfo, height, from, to);

            return GetSubImage(from, to - from, videochannel);
        }
        return 0;
    }


    ///////////////////////////////////////////
    // svlSampleImageCustom specific methods //
    ///////////////////////////////////////////

    template <class __ownerType>
    int SetMatrix(vctDynamicMatrixBase<__ownerType, _ValueType>& matrix, unsigned int videochannel = 0)
    {
        if (!OwnData && videochannel < _VideoChannels) {
            Image[videochannel].SetRef(matrix);
#if (CISST_SVL_HAS_OPENCV == ON)
            if (OCVImageHeader[videochannel]) {
                cvInitImageHeader(OCVImageHeader[videochannel],
                                  cvSize(GetWidth(videochannel), GetHeight(videochannel)),
                                  GetOCVDepth(),
                                  _DataChannels);
                cvSetData(OCVImageHeader[videochannel],
                          GetPointer(videochannel),
                          GetWidth(videochannel) * _DataChannels);
            }
#endif // CISST_SVL_HAS_OPENCV
            return SVL_OK;
        }
        return SVL_FAIL;
    }

    vctDynamicMatrixRef<_ValueType> GetMatrixRef(const unsigned int videochannel = 0)
    {
        if (videochannel < _VideoChannels) return Image[videochannel];
        else return InvalidMatrix;
    }

    const vctDynamicMatrixRef<_ValueType> GetMatrixRef(const unsigned int videochannel = 0) const
    {
        if (videochannel < _VideoChannels) return Image[videochannel];
        else return InvalidMatrix;
    }

    _ValueType* GetPointer(const unsigned int videochannel = 0)
    {
        if (videochannel < _VideoChannels) return Image[videochannel].Pointer();
        return 0;
    }

    const _ValueType* GetPointer(const unsigned int videochannel = 0) const
    {
        if (videochannel < _VideoChannels) return Image[videochannel].Pointer();
        return 0;
    }

    _ValueType* GetPointer(const unsigned int videochannel, const unsigned int x, const unsigned int y)
    {
        if (videochannel < _VideoChannels) {
            return Image[videochannel].Pointer(y, x * _DataChannels);
        }
        return 0;
    }

    const _ValueType* GetPointer(const unsigned int videochannel, const unsigned int x, const unsigned int y) const
    {
        if (videochannel < _VideoChannels) {
            return Image[videochannel].Pointer(y, x * _DataChannels);
        }
        return 0;
    }

private:
    bool OwnData;
    vctDynamicMatrixRef<_ValueType> Image[_VideoChannels];
    vctDynamicMatrix<_ValueType>*   OwnImage[_VideoChannels];
    vctDynamicMatrix<_ValueType>    InvalidMatrix;

#if (CISST_SVL_HAS_OPENCV == ON)
    IplImage* OCVImageHeader[_VideoChannels];

    int GetOCVDepth()
    {
        if (IsTypeUInt8 <_ValueType>(static_cast<_ValueType>(0))) return IPL_DEPTH_8U;
        if (IsTypeUInt16<_ValueType>(static_cast<_ValueType>(0))) return IPL_DEPTH_16U;
        if (IsTypeFloat <_ValueType>(static_cast<_ValueType>(0))) return IPL_DEPTH_32F;
        return -1;
    }
#endif // CISST_SVL_HAS_OPENCV
};

#endif // _svlSampleImageCustom_h

